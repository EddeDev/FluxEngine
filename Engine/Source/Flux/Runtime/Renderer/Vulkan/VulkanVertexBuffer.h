#pragma once

#include "Flux/Runtime/Renderer/VertexBuffer.h"

#include "Vulkan.h"
#include "VulkanResourceAllocator.h"

namespace Flux {

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(uint32 size);
		VulkanVertexBuffer(const void* data, uint32 size);
		virtual ~VulkanVertexBuffer();

		virtual void SetData(Ref<CommandBuffer> commandBuffer, const void* data, uint32 size, uint32 offset = 0) override;
		virtual void RT_SetData(Ref<CommandBuffer> commandBuffer, const void* data, uint32 size, uint32 offset = 0) override;

		virtual void Bind(Ref<CommandBuffer> commandBuffer) const override;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer) const override;
	private:
		uint8* m_Storage = nullptr;
		uint32 m_Size = 0;

		VkBuffer m_Buffer = VK_NULL_HANDLE;
		ResourceAllocation m_Allocation = nullptr;
	};

}