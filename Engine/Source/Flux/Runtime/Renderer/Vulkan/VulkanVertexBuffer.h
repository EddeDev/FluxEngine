#pragma once

#include "Flux/Runtime/Renderer/VertexBuffer.h"

#include "Vulkan.h"

namespace Flux {

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(const void* data, uint32 size);
		virtual ~VulkanVertexBuffer();

		virtual void Bind(Ref<CommandBuffer> commandBuffer) const override;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer) const override;
	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkBuffer m_StagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
		VkDeviceMemory m_StagingBufferMemory = VK_NULL_HANDLE;
	};

}