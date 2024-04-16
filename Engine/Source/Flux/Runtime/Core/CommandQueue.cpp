#include "FluxPCH.h"
#include "CommandQueue.h"

namespace Flux {

	CommandQueue::CommandQueue(const std::string& debugName, uint64 initialSize)
		: m_DebugName(debugName)
	{
		m_Buffer.Allocate(initialSize);
		m_BufferPointer = m_Buffer.GetData<uint8>();
	}

	CommandQueue::~CommandQueue()
	{
		m_Buffer.Release();
	}

	void CommandQueue::Flush()
	{
		uint8* data = m_Buffer.GetData<uint8>();
		while (data != m_BufferPointer)
		{
			CommandFn func = *(CommandFn*)data;
			data += sizeof(CommandFn);

			uint32 size = *(uint32*)data;
			data += sizeof(uint32);

			func(data);
			data += size;
		}

		if (m_ShouldShrink)
		{
			const uint32 currentSize = static_cast<uint32>(m_BufferPointer - m_Buffer.GetData<uint8>());
			Resize(0, currentSize / 2);
			m_ShouldShrink = false;
		}

		m_BufferPointer = m_Buffer.GetData<uint8>();
	}

	void* CommandQueue::Allocate(CommandFn func, uint32 size)
	{
		const uint32 currentOffset = static_cast<uint32>(m_BufferPointer - m_Buffer.GetData<uint8>());
		const uint32 newSize = currentOffset + sizeof(CommandFn) + sizeof(uint32) + size;
		if (newSize > m_Buffer.Size)
		{
			uint32 newCapacity = m_Buffer.Size * 2;
			while (newCapacity < newSize)
				newCapacity *= 2;

			Resize(currentOffset, newCapacity);
		}

		*(CommandFn*)m_BufferPointer = func;
		m_BufferPointer += sizeof(CommandFn);

		*(uint32*)m_BufferPointer = size;
		m_BufferPointer += sizeof(uint32);

		uint8* buffer = m_BufferPointer;
		m_BufferPointer += size;
		return buffer;
	}

	void CommandQueue::Resize(uint32 currentOffset, uint32 newCapacity)
	{
		FLUX_WARNING("{0} -  Resize({1}, {2})", m_DebugName, currentOffset, newCapacity);

		m_Buffer.Reallocate(newCapacity);
		m_BufferPointer = m_Buffer.GetData<uint8>(currentOffset);
	}

}