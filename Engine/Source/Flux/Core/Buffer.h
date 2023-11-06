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
			free(Data);
			Data = malloc(size);
			Size = size;
		}

		void Release()
		{
			free(Data);
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
		std::mutex BufferPoolMutex;
	public:
		RenderThreadStorage()
		{
			BufferPool.resize(2);
		}

		~RenderThreadStorage()
		{
			BufferPoolMutex.lock();
			FLUX_INFO("[Render Thread Storage {0}] Releasing {1} buffers", (void*)this, BufferPool.size());
			for (auto& buffer : BufferPool)
				buffer.Buffer.Release();
			BufferPoolMutex.unlock();
		}

		void Allocate(uint32 size)
		{
			BufferPoolMutex.lock();

			FLUX_INFO("[Render Thread Storage {0}] Allocating {1} bytes ({2} buffers)", (void*)this, size, BufferPool.size());

			Size = size;

			for (auto& buffer : BufferPool)
			{
				buffer.Buffer.Allocate(size);
				buffer.Buffer.FillWithZeros();
			}

			BufferPoolMutex.unlock();
		}

		uint32 SetData(const void* data, uint32 size, uint64 offset = 0)
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
				FLUX_WARNING("[Render Thread Storage {0}] Resizing buffer pool to {1}", (void*)this, BufferPool.size() * 2);
				BufferPool.resize(BufferPool.size() * 2);
				bufferIndex = static_cast<int32>(BufferPool.size() / 2);
			}

			FLUX_INFO("[Render Thread Storage {0}] Setting data to buffer index {1}", (void*)this, bufferIndex);

			auto& buffer = BufferPool[bufferIndex];
			buffer.IsAvailable = false;

			if (!buffer.Buffer)
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
			FLUX_INFO("[Render Thread Storage {0}] Setting buffer state at index {1} to Available", (void*)this, bufferIndex);
			BufferPool[bufferIndex].IsAvailable = true;
			BufferPoolMutex.unlock();
		}

		Buffer GetBuffer(uint32 bufferIndex)
		{
			BufferPoolMutex.lock();
			Buffer buffer = BufferPool.at(bufferIndex).Buffer;
			BufferPoolMutex.unlock();
			return buffer;
		}
	};

}