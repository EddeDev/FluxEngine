#include "FluxPCH.h"
#include "VulkanVertexBuffer.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"

namespace Flux {

	VulkanVertexBuffer::VulkanVertexBuffer(uint32 size)
		: m_Size(size)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Storage = new uint8[size];
		memset(m_Storage, 0, size);

		Ref<VulkanVertexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();

			auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();

			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = instance->m_Size;
			bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

			instance->m_Allocation = allocator.CreateBuffer(bufferCreateInfo, ResourceMemoryUsage::CpuToGpu, instance->m_Buffer);
		});
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const void* data, uint32 size)
		: m_Size(size)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Storage = new uint8[size];
		memcpy(m_Storage, data, size);

		Ref<VulkanVertexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();

			auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();

			VkBufferCreateInfo stagingBufferCreateInfo = {};
			stagingBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			stagingBufferCreateInfo.size = instance->m_Size;
			stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			stagingBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VkBuffer stagingBuffer;
			ResourceAllocation stagingBufferAllocation = allocator.CreateBuffer(stagingBufferCreateInfo, ResourceMemoryUsage::CpuToGpu, stagingBuffer);
	
			uint8* memory = allocator.MapMemory<uint8>(stagingBufferAllocation);
			memcpy(memory, instance->m_Storage, instance->m_Size);
			allocator.UnmapMemory(stagingBufferAllocation);

			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = instance->m_Size;
			bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

			instance->m_Allocation = allocator.CreateBuffer(bufferCreateInfo, ResourceMemoryUsage::GpuOnly, instance->m_Buffer);

			VkCommandBuffer commandBuffer = VulkanDevice::Get()->GetCommandBuffer(true);

			VkBufferCopy copyRegion = {};
			copyRegion.size = instance->m_Size;
			vkCmdCopyBuffer(commandBuffer, stagingBuffer, instance->m_Buffer, 1, &copyRegion);

			VulkanDevice::Get()->FlushCommandBuffer(commandBuffer);

			allocator.DestroyBuffer(stagingBuffer, stagingBufferAllocation);
		});
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		delete[] m_Storage;

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([buffer = m_Buffer, allocation = m_Allocation]()
		{
			auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();
			allocator.DestroyBuffer(buffer, allocation);
		});
	}

	void VulkanVertexBuffer::SetData(Ref<CommandBuffer> commandBuffer, const void* data, uint32 size, uint32 offset)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		memcpy(m_Storage, (uint8*)data + offset, size);

		Ref<VulkanVertexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer, size, offset]() mutable
		{
			instance->RT_SetData(commandBuffer, instance->m_Storage, size, offset);
		});
	}

	void VulkanVertexBuffer::RT_SetData(Ref<CommandBuffer> commandBuffer, const void* data, uint32 size, uint32 offset)
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();
		uint8* memory = allocator.MapMemory<uint8>(m_Allocation);
		memcpy(memory, (uint8*)data + offset, size);
		allocator.UnmapMemory(m_Allocation);
	}

	void VulkanVertexBuffer::Bind(Ref<CommandBuffer> commandBuffer) const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<const VulkanVertexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer]()
		{
			instance->RT_Bind(commandBuffer);
		});
	}

	void VulkanVertexBuffer::RT_Bind(Ref<CommandBuffer> commandBuffer) const
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		static VkDeviceSize offsets[1] = { 0 };

		vkCmdBindVertexBuffers(
			commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer(), 
			0, 
			1, 
			&m_Buffer, 
			offsets
		);
	}

}