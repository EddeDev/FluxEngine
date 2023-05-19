#pragma once

#include "Flux/Runtime/Renderer/CommandBuffer.h"

#include "Vulkan.h"

namespace Flux {

	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(const CommandBufferCreateInfo& createInfo);
		virtual ~VulkanCommandBuffer();

		virtual void Begin() override;
		virtual void RT_Begin() override;

		virtual void End() override;
		virtual void RT_End() override;

		virtual void Submit() override;
		virtual void RT_Submit() override;

		VkCommandBuffer GetCommandBuffer(uint32 frameIndex) const
		{
			FLUX_ASSERT(frameIndex < m_CommandBuffers.size());
			return m_CommandBuffers.at(frameIndex);
		}

		VkCommandBuffer GetActiveCommandBuffer() const { return m_ActiveCommandBuffer; }
	private:
		CommandBufferCreateInfo m_CreateInfo;
		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<VkFence> m_Fences;
		VkCommandBuffer m_ActiveCommandBuffer = VK_NULL_HANDLE;
	};

}