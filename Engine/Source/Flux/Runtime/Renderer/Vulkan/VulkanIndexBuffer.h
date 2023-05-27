#pragma once

#include "Flux/Runtime/Renderer/IndexBuffer.h"

#include "Vulkan.h"
#include "VulkanResourceAllocator.h"

namespace Flux {

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(const void* data, uint32 size);
		virtual ~VulkanIndexBuffer();

		virtual void Bind(Ref<CommandBuffer> commandBuffer) const override;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer) const override;

		virtual uint32 GetCount() const override { return m_Count; }
	private:
		uint8* m_Storage = nullptr;
		uint32 m_Size = 0, m_Count = 0;

		VkBuffer m_Buffer = VK_NULL_HANDLE;
		ResourceAllocation m_Allocation = nullptr;
	};

}