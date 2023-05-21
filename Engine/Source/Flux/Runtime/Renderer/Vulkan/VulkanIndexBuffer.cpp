#include "FluxPCH.h"
#include "VulkanIndexBuffer.h"

#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"

namespace Flux {

	VulkanIndexBuffer::VulkanIndexBuffer(const void* data, uint32 size)
	{
		VkDevice device = VulkanDevice::Get()->GetDevice();

		// Create staging buffer
		{
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = size;
			bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			VK_CHECK(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &m_StagingBuffer));

			VkMemoryRequirements memoryRequirments;
			vkGetBufferMemoryRequirements(device, m_StagingBuffer, &memoryRequirments);

			VkMemoryAllocateInfo memoryAllocateInfo = {};
			memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memoryAllocateInfo.allocationSize = memoryRequirments.size;
			memoryAllocateInfo.memoryTypeIndex = VulkanDevice::Get()->GetMemoryTypeIndex(memoryRequirments.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			VK_CHECK(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &m_StagingBufferMemory));
			VK_CHECK(vkBindBufferMemory(device, m_StagingBuffer, m_StagingBufferMemory, 0));

			void* dstData;
			VK_CHECK(vkMapMemory(device, m_StagingBufferMemory, 0, memoryAllocateInfo.allocationSize, 0, &dstData));
			memcpy(dstData, data, size);
			vkUnmapMemory(device, m_StagingBufferMemory);
		}

		// Create local buffer
		{
			VkBufferCreateInfo bufferCreateInfo = {};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.size = size;
			bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			VK_CHECK(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &m_Buffer));

			VkMemoryRequirements memoryRequirments;
			vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirments);

			VkMemoryAllocateInfo memoryAllocateInfo = {};
			memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memoryAllocateInfo.allocationSize = memoryRequirments.size;
			memoryAllocateInfo.memoryTypeIndex = VulkanDevice::Get()->GetMemoryTypeIndex(memoryRequirments.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			VK_CHECK(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &m_BufferMemory));
			VK_CHECK(vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0));

			VkCommandBuffer commandBuffer = VulkanDevice::Get()->GetCommandBuffer(true);
			
			VkBufferCopy copyRegion = {};
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, m_StagingBuffer, m_Buffer, 1, &copyRegion);

			VulkanDevice::Get()->FlushCommandBuffer(commandBuffer);
		}
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([buffer = m_Buffer, bufferMemory = m_BufferMemory, stagingBuffer = m_StagingBuffer, stagingBufferMemory = m_StagingBufferMemory]()
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();
			
			vkDestroyBuffer(device, buffer, nullptr);
			vkFreeMemory(device, bufferMemory, nullptr);

			vkDestroyBuffer(device, stagingBuffer, nullptr);
			vkFreeMemory(device, stagingBufferMemory, nullptr);
		});
	}

	void VulkanIndexBuffer::Bind(Ref<CommandBuffer> commandBuffer) const
	{
		Ref<const VulkanIndexBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer]()
		{
			instance->RT_Bind(commandBuffer);
		});
	}

	void VulkanIndexBuffer::RT_Bind(Ref<CommandBuffer> commandBuffer) const
	{
		static VkDeviceSize offsets[1] = { 0 };

		vkCmdBindIndexBuffer(
			commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer(),
			m_Buffer,
			0,
			VK_INDEX_TYPE_UINT32 /* TODO: IndexBufferDataType */
		);
	}

}