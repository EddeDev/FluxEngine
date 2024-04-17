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

		bool Reallocate(uint64_t size)
		{
			void* data = realloc(Data, size);
			if (data)
			{
				Data = data;
				Size = size;
				return true;
			}
			return false;
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

		void SetData(const void* data, uint64 size, uint64 offset = 0)
		{
			memcpy((uint8*)Data + offset, data, size);
		}

		template<typename T = const void>
		T* GetData(uint64 offset = 0) const
		{
			return (T*)((uint8*)Data + offset);
		}

		operator bool() const { return Data != nullptr; }
	};

	class RenderThreadStorage
	{
	public:
		RenderThreadStorage()
		{
			m_BufferPool.resize(2);
		}

		~RenderThreadStorage()
		{
			std::lock_guard<std::mutex> lock(m_BufferPoolMutex);
			for (auto& buffer : m_BufferPool)
				buffer.Buffer.Release();
		}

		uint32 SetData(const void* data, uint64 size)
		{
			std::lock_guard<std::mutex> lock(m_BufferPoolMutex);

			int32 bufferIndex = -1;
			for (int32 i = 0; i < static_cast<int32>(m_BufferPool.size()); i++)
			{
				if (m_BufferPool[i].IsAvailable)
				{
					bufferIndex = i;
					break;
				}
			}

			if (bufferIndex == -1)
			{
				bufferIndex = static_cast<int32>(m_BufferPool.size());
				m_BufferPool.resize(m_BufferPool.size() * 2);
			}

			auto& buffer = m_BufferPool[bufferIndex];
			buffer.IsAvailable = false;
			FLUX_VERIFY(buffer.UsedSize <= buffer.Buffer.Size);

			if (size > buffer.Buffer.Size)
			{
				// FLUX_INFO_CATEGORY("Render Thread Storage", "Reallocating buffer at index {0} from {1} bytes to {2} bytes", bufferIndex, buffer.Buffer.Size, size);
				bool success = buffer.Buffer.Reallocate(size);
				FLUX_VERIFY(success);
			}

			buffer.UsedSize = size;
			buffer.Buffer.SetData(data, size);
			return bufferIndex;
		}

		uint32 SetData(const Buffer& buffer)
		{
			return SetData(buffer.Data, buffer.Size);
		}

		void SetBufferAvailable(uint32 bufferIndex)
		{
			std::lock_guard<std::mutex> lock(m_BufferPoolMutex);

			auto& buffer = m_BufferPool[bufferIndex];
			buffer.IsAvailable = true;
		}

		Buffer GetBuffer(uint32 bufferIndex) const
		{
			std::lock_guard<std::mutex> lock(m_BufferPoolMutex);

			Buffer buffer;
			buffer.Data = m_BufferPool[bufferIndex].Buffer.Data;
			buffer.Size = m_BufferPool[bufferIndex].UsedSize;
			return buffer;
		}
	private:
		struct BufferData
		{
			Flux::Buffer Buffer;
			uint32 UsedSize = 0;
			bool IsAvailable = true;
		};

		std::vector<BufferData> m_BufferPool;
		mutable std::mutex m_BufferPoolMutex;
	};

}