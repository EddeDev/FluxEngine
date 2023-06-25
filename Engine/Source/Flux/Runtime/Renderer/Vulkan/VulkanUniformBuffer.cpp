#include "FluxPCH.h"
#include "VulkanUniformBuffer.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"

namespace Flux {

	VulkanUniformBuffer::VulkanUniformBuffer(const UniformBufferCreateInfo& createInfo)
		: m_CreateInfo(createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_BufferPool.resize(2);

		Ref<VulkanUniformBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();

			auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();

			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = instance->m_CreateInfo.Size;
			bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

			instance->m_Allocation = allocator.CreateBuffer(bufferCreateInfo, ResourceMemoryUsage::CpuToGpu, instance->m_Buffer);
		});
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		std::lock_guard<std::mutex> lock(m_BufferPoolMutex);

		for (auto& buffer : m_BufferPool)
			delete[] buffer.Storage;

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([buffer = m_Buffer, allocation = m_Allocation]()
		{
			auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();
			allocator.DestroyBuffer(buffer, allocation);
		});
	}

	void VulkanUniformBuffer::SetData(const void* data, uint32 size, uint32 offset)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		std::lock_guard<std::mutex> lock(m_BufferPoolMutex);

		uint32 bufferIndex = std::numeric_limits<uint32>::max();
		for (uint32 i = 0; i < static_cast<uint32>(m_BufferPool.size()); i++)
		{
			if (m_BufferPool[i].State == BufferState::Available)
			{
				bufferIndex = i;
				break;
			}
		}

		if (bufferIndex == std::numeric_limits<uint32>::max())
		{
			m_BufferPool.resize(m_BufferPool.size() * 2);
			bufferIndex = static_cast<uint32>(m_BufferPool.size() / 2);
		}

		auto& buffer = m_BufferPool[bufferIndex];
		buffer.State = BufferState::InUse;

		if (!buffer.Storage)
		{
			buffer.Storage = new uint8[m_CreateInfo.Size];
			memset(buffer.Storage, 0, m_CreateInfo.Size);
		}

		memcpy(buffer.Storage, (uint8*)data + offset, size);

		Ref<VulkanUniformBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, bufferIndex, size, offset]() mutable
		{
			std::lock_guard<std::mutex> lock(instance->m_BufferPoolMutex);

			auto& buffer = instance->m_BufferPool[bufferIndex];
			if (!buffer.Storage)
				return;

			if (buffer.State == BufferState::Available)
				FLUX_VERIFY(false);

			auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();
			uint8* memory = allocator.MapMemory<uint8>(instance->m_Allocation);
			memcpy(memory + offset, buffer.Storage + offset, size);
			allocator.UnmapMemory(instance->m_Allocation);

			buffer.State = BufferState::Available;
		});
	}

	void VulkanUniformBuffer::RT_SetData(const void* data, uint32 size, uint32 offset)
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();
		uint8* memory = allocator.MapMemory<uint8>(m_Allocation);
		memcpy(memory, (uint8*)data + offset, size);
		allocator.UnmapMemory(m_Allocation);
	}

}