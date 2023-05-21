#include "FluxPCH.h"
#include "VulkanFramebuffer.h"

#include "Flux/Runtime/Engine/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"

namespace Flux {

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferCreateInfo& createInfo)
		: m_CreateInfo(createInfo), m_Width(createInfo.Width), m_Height(createInfo.Height)
	{
		if (m_Width == 0 || m_Height == 0)
		{
			m_Width = Engine::Get().GetSwapchain()->GetWidth();
			m_Height = Engine::Get().GetSwapchain()->GetHeight();
		}
		
		Resize(m_Width, m_Height, true);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}

	void VulkanFramebuffer::Resize(uint32 width, uint32 height, bool forceRecreate)
	{
		if (!forceRecreate && (m_Width == width && m_Height == height))
			return;

		m_Width = width;
		m_Height = height;

		if (!m_CreateInfo.SwapchainTarget)
			Invalidate();
	}

	void VulkanFramebuffer::Invalidate()
	{
		// TODO
	}

	void VulkanFramebuffer::Bind(Ref<CommandBuffer> commandBuffer) const
	{
		Ref<const VulkanFramebuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer]()
		{
			instance->RT_Bind(commandBuffer);
		});
	}

	void VulkanFramebuffer::RT_Bind(Ref<CommandBuffer> commandBuffer) const
	{
		VkCommandBuffer activeCommandBuffer = commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer();

		VkClearValue clearValues[2];
		clearValues[0].color = *(VkClearColorValue*)&m_CreateInfo.ClearColor[0];
		clearValues[1].depthStencil = { m_CreateInfo.DepthClearValue, m_CreateInfo.StencilClearValue };

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

		if (m_CreateInfo.SwapchainTarget)
		{
			Ref<VulkanSwapchain> swapchain = Engine::Get().GetSwapchain().As<VulkanSwapchain>();

			uint32 width = swapchain->GetWidth();
			uint32 height = swapchain->GetHeight();

			renderPassBeginInfo.renderArea.offset.x = 0;
			renderPassBeginInfo.renderArea.offset.y = 0;
			renderPassBeginInfo.renderArea.extent.width = width;
			renderPassBeginInfo.renderArea.extent.height = height;
			renderPassBeginInfo.renderPass = swapchain->GetRenderPass();
			renderPassBeginInfo.framebuffer = swapchain->GetFramebuffer(Renderer::RT_GetCurrentFrameIndex());

			VkViewport viewport;
			viewport.x = 0.0f;
			viewport.y = (float)height;
			viewport.width = (float)width;
			viewport.height = -(float)height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(activeCommandBuffer, 0, 1, &viewport);

			VkRect2D scissor;
			scissor.offset = { 0, 0 };
			scissor.extent = { width, height };
			vkCmdSetScissor(activeCommandBuffer, 0, 1, &scissor);
		}
		else
		{
			// TODO
		}

		vkCmdBeginRenderPass(activeCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanFramebuffer::Unbind(Ref<CommandBuffer> commandBuffer) const
	{
		Ref<const VulkanFramebuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer]()
		{
			instance->RT_Unbind(commandBuffer);
		});
	}

	void VulkanFramebuffer::RT_Unbind(Ref<CommandBuffer> commandBuffer) const
	{
		vkCmdEndRenderPass(commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer());
	}

	uint32 VulkanFramebuffer::GetWidth() const
	{
		if (m_CreateInfo.SwapchainTarget)
			return Engine::Get().GetSwapchain().As<VulkanSwapchain>()->GetWidth();
		return m_Width;
	}

	uint32 VulkanFramebuffer::GetHeight() const
	{
		if (m_CreateInfo.SwapchainTarget)
			return Engine::Get().GetSwapchain().As<VulkanSwapchain>()->GetHeight();
		return m_Height;
	}

	VkRenderPass VulkanFramebuffer::GetRenderPass() const
	{
		if (m_CreateInfo.SwapchainTarget)
			return Engine::Get().GetSwapchain().As<VulkanSwapchain>()->GetRenderPass();
		return m_RenderPass;
	}

}