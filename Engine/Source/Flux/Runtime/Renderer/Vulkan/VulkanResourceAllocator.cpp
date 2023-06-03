#include "FluxPCH.h"
#include "VulkanResourceAllocator.h"

#include "VulkanDevice.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace Flux {

	VulkanResourceAllocator::VulkanResourceAllocator()
	{
		Ref<VulkanDevice> device = VulkanDevice::Get();
		Ref<VulkanAdapter> adapter = device->GetAdapter().As<VulkanAdapter>();
		Ref<VulkanContext> context = adapter->GetContext().As<VulkanContext>();

		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		allocatorCreateInfo.physicalDevice = adapter->GetPhysicalDevice();
		allocatorCreateInfo.device = device->GetDevice();
		allocatorCreateInfo.instance = context->GetInstance();

		VK_CHECK(vmaCreateAllocator(&allocatorCreateInfo, (VmaAllocator*)&m_Allocator));
	}

	VulkanResourceAllocator::~VulkanResourceAllocator()
	{
		vmaDestroyAllocator((VmaAllocator)m_Allocator);
	}

	ResourceAllocation VulkanResourceAllocator::CreateBuffer(const VkBufferCreateInfo& createInfo, ResourceMemoryUsage usage, VkBuffer& outBuffer)
	{
		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = (VmaMemoryUsage)usage;

		VmaAllocation allocation;

		VkResult result = vmaCreateBuffer((VmaAllocator)m_Allocator, &createInfo, &allocationCreateInfo, &outBuffer, &allocation, nullptr);
		if (result != VK_SUCCESS)
		{
			FLUX_VERIFY(false);
			return nullptr;
		}
		
		return allocation;
	}

	ResourceAllocation VulkanResourceAllocator::CreateImage(const VkImageCreateInfo& createInfo, ResourceMemoryUsage usage, VkImage& outImage)
	{
		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = (VmaMemoryUsage)usage;

		VmaAllocation allocation;

		VkResult result = vmaCreateImage((VmaAllocator)m_Allocator, &createInfo, &allocationCreateInfo, &outImage, &allocation, nullptr);
		if (result != VK_SUCCESS)
		{
			FLUX_VERIFY(false);
			return nullptr;
		}

		return allocation;
	}

	void VulkanResourceAllocator::DestroyBuffer(VkBuffer buffer, ResourceAllocation allocation)
	{
		FLUX_ASSERT(allocation);

		vmaDestroyBuffer((VmaAllocator)m_Allocator, buffer, (VmaAllocation)allocation);
	}

	void VulkanResourceAllocator::DestroyImage(VkImage image, ResourceAllocation allocation)
	{
		FLUX_ASSERT(allocation);

		vmaDestroyImage((VmaAllocator)m_Allocator, image, (VmaAllocation)allocation);
	}

	void VulkanResourceAllocator::MapMemory(ResourceAllocation allocation, void** outData) const
	{
		FLUX_ASSERT(allocation);

		VK_CHECK(vmaMapMemory((VmaAllocator)m_Allocator, (VmaAllocation)allocation, outData));
	}

	void VulkanResourceAllocator::UnmapMemory(ResourceAllocation allocation) const
	{
		FLUX_ASSERT(allocation);

		vmaUnmapMemory((VmaAllocator)m_Allocator, (VmaAllocation)allocation);
	}

}