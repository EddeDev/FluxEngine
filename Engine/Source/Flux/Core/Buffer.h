#pragma once

#include "BaseTypes.h"

namespace Flux {

	struct Buffer
	{
		void* Data;
		uint64 Size;

		Buffer()
			: Data(nullptr), Size(0) {}
		Buffer(void* data, uint64 size)
			: Data(data), Size(size) {}

		static Buffer Copy(Buffer other)
		{
			Buffer buffer;
			buffer.Allocate(other.Size);
			memcpy(buffer.Data, other.Data, other.Size);
			return buffer;
		}

		static Buffer Copy(const void* data, uint64 size)
		{
			Buffer buffer;
			buffer.Allocate(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}

		void Allocate(uint64 size)
		{
			Release();

			if (size > 0)
			{
				Data = malloc(size);
				Size = size;
			}
		}

		void Release()
		{
			free(Data);
			Data = nullptr;

			Size = 0;
		}

		void FillWithZeros()
		{
			memset(Data, 0, Size);
		}

		void* GetData(uint64 offset = 0) const
		{
			return (uint8*)Data + offset;
		}

		operator bool() const { return Data != nullptr; }
	};

	struct RenderThreadStorage
	{
	private:
		struct BufferData
		{
			Flux::Buffer Buffer;
			bool IsAvailable = true;
		};

		uint64 Size = 0;

		std::vector<BufferData> BufferPool;
		mutable std::mutex BufferPoolMutex;
	public:
		RenderThreadStorage()
		{
			BufferPool.resize(2);
		}

		~RenderThreadStorage()
		{
			BufferPoolMutex.lock();
			for (auto& buffer : BufferPool)
				buffer.Buffer.Release();
			BufferPoolMutex.unlock();
		}

		void SetSize(uint64 size)
		{
			BufferPoolMutex.lock();
			Size = size;
			BufferPoolMutex.unlock();
		}

		uint32 SetData(const void* data, uint64 size, uint64 offset = 0)
		{
			BufferPoolMutex.lock();

			int32 bufferIndex = -1;
			for (int32 i = 0; i < static_cast<int32>(BufferPool.size()); i++)
			{
				if (BufferPool[i].IsAvailable)
				{
					bufferIndex = i;
					break;
				}
			}

			if (bufferIndex == -1)
			{
				BufferPool.resize(BufferPool.size() * 2);
				bufferIndex = static_cast<int32>(BufferPool.size() / 2);
			}

			auto& buffer = BufferPool[bufferIndex];
			buffer.IsAvailable = false;

			if (buffer.Buffer.Size != Size)
			{
				buffer.Buffer.Allocate(Size);
				buffer.Buffer.FillWithZeros();
			}

			memcpy(buffer.Buffer.Data, (uint8*)data + offset, size);
			BufferPoolMutex.unlock();
			return bufferIndex;
		}

		void SetBufferAvailable(uint32 bufferIndex)
		{
			BufferPoolMutex.lock();
			BufferPool[bufferIndex].IsAvailable = true;
			BufferPoolMutex.unlock();
		}

		Buffer GetBuffer(uint32 bufferIndex) const
		{
			BufferPoolMutex.lock();
			Buffer buffer = BufferPool.at(bufferIndex).Buffer;
			BufferPoolMutex.unlock();
			return buffer;
		}

		uint64 GetSize() const
		{
			BufferPoolMutex.lock();
			uint64 size = Size;
			BufferPoolMutex.unlock();
			return size;
		}
	};

}