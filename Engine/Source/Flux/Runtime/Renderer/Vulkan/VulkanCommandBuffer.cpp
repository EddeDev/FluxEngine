#include "FluxPCH.h"
#include "VulkanCommandBuffer.h"

#include "Flux/Runtime/Engine/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

namespace Flux {

	VulkanCommandBuffer::VulkanCommandBuffer(const CommandBufferCreateInfo& createInfo)
		: m_CreateInfo(createInfo)
	{
		FLUX_INFO("Creating command buffer: {0}", createInfo.DebugLabel);
		if (!createInfo.CreateFromSwapchain)
		{
			FLUX_INFO("  Count: {0}", createInfo.Count);
			FLUX_INFO("  Transient: {0}", createInfo.Transient);
		}
		FLUX_INFO("  Created from Swapchain: {0}", createInfo.CreateFromSwapchain);

		if (!createInfo.CreateFromSwapchain)
		{
			Ref<VulkanCommandBuffer> instance = this;
			FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
			{
				VkDevice device = VulkanDevice::Get()->GetDevice();

				VkCommandPoolCreateInfo commandPoolCreateInfo = {};
				commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				if (instance->m_CreateInfo.Transient)
					commandPoolCreateInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
				commandPoolCreateInfo.queueFamilyIndex = VulkanDevice::Get()->GetQueueFamilyIndices().Graphics;
				VK_CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &instance->m_CommandPool));

				VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
				commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				commandBufferAllocateInfo.commandPool = instance->m_CommandPool;
				commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				commandBufferAllocateInfo.commandBufferCount = instance->m_CreateInfo.Count;

				instance->m_CommandBuffers.resize(static_cast<size_t>(instance->m_CreateInfo.Count));
				VK_CHECK(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, instance->m_CommandBuffers.data()));

				VkFenceCreateInfo fenceCreateInfo = {};
				fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				instance->m_Fences.resize(static_cast<size_t>(instance->m_CreateInfo.Count));
				for (uint32 i = 0; i < instance->m_CreateInfo.Count; i++)
					VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &instance->m_Fences[i]));
			});
		}
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		if (!m_CreateInfo.CreateFromSwapchain)
		{
			FLUX_SUBMIT_RENDER_COMMAND_RELEASE([commandPool = m_CommandPool, fences = m_Fences]()
			{
				VkDevice device = VulkanDevice::Get()->GetDevice();
				for (auto& fence : fences)
					vkDestroyFence(device, fence, nullptr);
				vkDestroyCommandPool(device, commandPool, nullptr);
			});
		}
	}

	void VulkanCommandBuffer::Begin()
	{
		Ref<VulkanCommandBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_Begin();
		});
	}

	void VulkanCommandBuffer::RT_Begin()
	{
		FLUX_ASSERT(!m_ActiveCommandBuffer);

		uint32 frameIndex = Renderer::RT_GetCurrentFrameIndex();

		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		if (m_CreateInfo.CreateFromSwapchain)
		{
			Ref<VulkanSwapchain> swapchain = Engine::Get().GetSwapchain().As<VulkanSwapchain>();
			commandBuffer = swapchain->GetCommandBuffer(frameIndex);
		}
		else
		{
			commandBuffer = m_CommandBuffers[frameIndex];
		}
		FLUX_ASSERT(commandBuffer);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

		m_ActiveCommandBuffer = commandBuffer;
	}

	void VulkanCommandBuffer::End()
	{
		Ref<VulkanCommandBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_End();
		});
	}

	void VulkanCommandBuffer::RT_End()
	{
		FLUX_ASSERT(m_ActiveCommandBuffer);

		VK_CHECK(vkEndCommandBuffer(m_ActiveCommandBuffer));
		m_ActiveCommandBuffer = VK_NULL_HANDLE;
	}

	void VulkanCommandBuffer::Submit()
	{
		Ref<VulkanCommandBuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_Submit();
		});
	}

	void VulkanCommandBuffer::RT_Submit()
	{
		// FLUX_ASSERT(!m_CreateInfo.CreateFromSwapchain);

		if (!m_CreateInfo.CreateFromSwapchain)
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();

			uint32 frameIndex = Renderer::RT_GetCurrentFrameIndex();

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = static_cast<uint32>(m_CommandBuffers.size());
			submitInfo.pCommandBuffers = &m_CommandBuffers[frameIndex];

			VK_CHECK(vkWaitForFences(device, 1, &m_Fences[frameIndex], VK_TRUE, std::numeric_limits<uint64>::max()));
			VK_CHECK(vkResetFences(device, 1, &m_Fences[frameIndex]));

			VK_CHECK(vkQueueSubmit(VulkanDevice::Get()->GetGraphicsQueue(), 1, &submitInfo, m_Fences[frameIndex]));
		}
	}

}