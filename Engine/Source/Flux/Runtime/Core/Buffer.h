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

		void SetSize(uint64 size)
		{
			std::lock_guard<std::mutex> lock(m_BufferPoolMutex);
			m_Size = size;
		}

		uint32 SetData(const void* data, uint64 size, uint64 offset = 0)
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
				m_BufferPool.resize(m_BufferPool.size() * 2);
				bufferIndex = static_cast<int32>(m_BufferPool.size() / 2);
			}

			auto& buffer = m_BufferPool[bufferIndex];
			buffer.IsAvailable = false;

			if (buffer.Buffer.Size != m_Size)
			{
				buffer.Buffer.Allocate(m_Size);
				buffer.Buffer.FillWithZeros();
			}

			memcpy(buffer.Buffer.Data, (uint8*)data + offset, size);
			return bufferIndex;
		}

		void SetBufferAvailable(uint32 bufferIndex)
		{
			std::lock_guard<std::mutex> lock(m_BufferPoolMutex);
			m_BufferPool[bufferIndex].IsAvailable = true;
		}

		const Buffer& GetBuffer(uint32 bufferIndex) const
		{
			std::lock_guard<std::mutex> lock(m_BufferPoolMutex);
			return m_BufferPool[bufferIndex].Buffer;
		}

		uint64 GetSize() const
		{
			std::lock_guard<std::mutex> lock(m_BufferPoolMutex);
			return m_Size;
		}
	private:
		struct BufferData
		{
			Flux::Buffer Buffer;
			bool IsAvailable = true;
		};

		uint64 m_Size = 0;

		std::vector<BufferData> m_BufferPool;
		mutable std::mutex m_BufferPoolMutex;
	};

}