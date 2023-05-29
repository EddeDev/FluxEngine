#pragma once

#include "Flux/Runtime/Renderer/ResourceAllocator.h"

#include "Vulkan.h"

namespace Flux {

	class VulkanResourceAllocator : public ResourceAllocator
	{
	public:
		VulkanResourceAllocator();
		virtual ~VulkanResourceAllocator();

		ResourceAllocation CreateBuffer(const VkBufferCreateInfo& createInfo, ResourceMemoryUsage usage, VkBuffer& outBuffer);
		ResourceAllocation CreateImage(const VkImageCreateInfo& createInfo, ResourceMemoryUsage usage, VkImage& outImage);
		void DestroyBuffer(VkBuffer buffer, ResourceAllocation allocation);
		void DestroyImage(VkImage image, ResourceAllocation allocation);

		void MapMemory(ResourceAllocation allocation, void** outData) const;

		template<typename T>
		T* MapMemory(ResourceAllocation allocation) const
		{
			T* mappedMemory;
			MapMemory(allocation, (void**)&mappedMemory);
			return mappedMemory;
		}

		void UnmapMemory(ResourceAllocation allocation) const;
	private:
		void* m_Allocator;
	};

}