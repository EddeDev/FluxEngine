#include "FluxPCH.h"
#include "Renderer.h"

#include "Flux/Runtime/Engine/Engine.h"

#include "Vulkan/Vulkan.h"
#include "Vulkan/VulkanSwapchain.h"

namespace Flux {

	void Renderer::Init()
	{
	}

	void Renderer::Shutdown()
	{
	}

	void Renderer::BeginFrame()
	{

	}

	void Renderer::EndFrame()
	{
	}

	void Renderer::BeginRenderPass()
	{
		Ref<VulkanSwapchain> swapchain = Engine::Get().GetSwapchain().As<VulkanSwapchain>();
		VkCommandBuffer commandBuffer = swapchain->GetCommandBuffer();

		uint32 width = swapchain->GetWidth();
		uint32 height = swapchain->GetHeight();

		VkClearValue clearValues[2];
		clearValues[0].color = { { 0.1f, 0.1f, 0.1f, 1.0f } };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = swapchain->GetRenderPass();
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = width;
		renderPassBeginInfo.renderArea.extent.height = height;
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;
		renderPassBeginInfo.framebuffer = swapchain->GetFramebuffer();

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)width;
		viewport.height = (float)height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor;
		scissor.offset = { 0, 0 };
		scissor.extent = { width, height };
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void Renderer::EndRenderPass()
	{
		Ref<VulkanSwapchain> swapchain = Engine::Get().GetSwapchain().As<VulkanSwapchain>();
		VkCommandBuffer commandBuffer = swapchain->GetCommandBuffer();
		vkCmdEndRenderPass(commandBuffer);
		VK_CHECK(vkEndCommandBuffer(commandBuffer));
	}

}