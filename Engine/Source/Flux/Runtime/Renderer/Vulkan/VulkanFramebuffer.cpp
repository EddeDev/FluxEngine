#include "FluxPCH.h"
#include "VulkanFramebuffer.h"

#include "Flux/Runtime/Engine/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"
#include "VulkanImage.h"

namespace Flux {

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferCreateInfo& createInfo)
		: m_CreateInfo(createInfo), m_Width(createInfo.Width), m_Height(createInfo.Height)
	{
		if (m_Width == 0 || m_Height == 0)
		{
			m_Width = Engine::Get().GetSwapchain()->GetWidth();
			m_Height = Engine::Get().GetSwapchain()->GetHeight();
		}
		
		auto& attachments = createInfo.Attachments.Attachments;

		uint32 attachmentIndex = 0;
		for (auto& attachment : attachments)
		{
			ImageCreateInfo attachmentCreateInfo;
			attachmentCreateInfo.Width = m_Width;
			attachmentCreateInfo.Height = m_Height;
			attachmentCreateInfo.Format = attachment.Format;
			attachmentCreateInfo.Usage = ImageUsage::Attachment;
			attachmentCreateInfo.DebugLabel = fmt::format("{0}[{1}/{2}]", createInfo.DebugLabel, attachmentIndex, Utils::IsDepthFormat(attachment.Format) ? "Depth Image" : "Color Image");

			Ref<Image2D> image = Image2D::Create(attachmentCreateInfo);
			if (Utils::IsDepthFormat(attachment.Format))
				m_DepthAttachment = image;
			else
				m_ColorAttachments.emplace_back(image);

			attachmentIndex++;
		}

		Resize(m_Width, m_Height, true);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([renderPass = m_RenderPass, framebuffer = m_Framebuffer]()
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();

			vkDestroyRenderPass(device, renderPass, nullptr);
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		});
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
		Ref<VulkanFramebuffer> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_Invalidate();
		});
	}

	void VulkanFramebuffer::RT_Invalidate()
	{
		VkDevice device = VulkanDevice::Get()->GetDevice();

		uint32 attachmentIndex = 0;
		for (auto& attachment : m_CreateInfo.Attachments.Attachments)
		{
			bool hasExistingImage = m_CreateInfo.ExistingImages.find(attachmentIndex) != m_CreateInfo.ExistingImages.end();
			auto* existingImage = hasExistingImage ? &m_CreateInfo.ExistingImages[attachmentIndex] : nullptr;

			if (Utils::IsDepthFormat(attachment.Format))
			{
				if (existingImage)
				{
					FLUX_VERIFY(Utils::IsDepthFormat(existingImage->Image->GetCreateInfo().Format));
					m_DepthAttachment = existingImage->Image;
				}
				else
				{
					m_DepthAttachment->GetCreateInfo().Width = m_Width;
					m_DepthAttachment->GetCreateInfo().Height = m_Height;
					m_DepthAttachment->RT_Invalidate();
				}

				if (existingImage)
				{
					if (existingImage->IsLayered)
						m_DepthAttachment->RT_AttachToFramebufferLayer(attachmentIndex, existingImage->Layer);
					else
						m_DepthAttachment->RT_AttachToFramebuffer(attachmentIndex);
				}
				else
				{
					m_DepthAttachment->RT_AttachToFramebuffer(attachmentIndex);
				}
			}
			else
			{
				if (existingImage)
				{
					FLUX_VERIFY(!Utils::IsDepthFormat(existingImage->Image->GetCreateInfo().Format));
					m_ColorAttachments[attachmentIndex] = existingImage->Image;
				}
				else
				{
					m_ColorAttachments[attachmentIndex]->GetCreateInfo().Width = m_Width;
					m_ColorAttachments[attachmentIndex]->GetCreateInfo().Height = m_Height;
					m_ColorAttachments[attachmentIndex]->RT_Invalidate();
				}

				if (existingImage)
				{
					if (existingImage->IsLayered)
						m_ColorAttachments[attachmentIndex]->RT_AttachToFramebufferLayer(attachmentIndex, existingImage->Layer);
					else
						m_ColorAttachments[attachmentIndex]->RT_AttachToFramebuffer(attachmentIndex);
				}
				else
				{
					m_ColorAttachments[attachmentIndex]->RT_AttachToFramebuffer(attachmentIndex);
				}
			}

			attachmentIndex++;
		}

		const uint32 attachmentCount = static_cast<uint32>(m_ColorAttachments.size()) + (m_DepthAttachment ? 1 : 0);
		FLUX_VERIFY(attachmentCount > 0);

		std::vector<VkAttachmentDescription> attachmentDescriptions;

		for (uint32 i = 0; i < static_cast<uint32>(m_ColorAttachments.size()); i++)
		{
			Ref<VulkanImage2D> image = m_ColorAttachments[i].As<VulkanImage2D>();

			auto& description = attachmentDescriptions.emplace_back();
			description.format = Utils::VulkanPixelFormat(image->GetCreateInfo().Format);
			description.samples = VK_SAMPLE_COUNT_1_BIT;
			description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			description.finalLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
		}

		if (m_DepthAttachment)
		{
			Ref<VulkanImage2D> image = m_DepthAttachment.As<VulkanImage2D>();

			auto& description = attachmentDescriptions.emplace_back();
			description.format = Utils::VulkanPixelFormat(image->GetCreateInfo().Format);
			description.samples = VK_SAMPLE_COUNT_1_BIT;
			description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		}

		FLUX_VERIFY(static_cast<uint32>(attachmentDescriptions.size()) == attachmentCount);

		std::vector<VkAttachmentReference> colorAttachmentReferences;
		VkAttachmentReference depthAttachmentReference = {};
		for (uint32 i = 0; i < attachmentCount; i++)
		{
			auto& attachment = m_CreateInfo.Attachments.Attachments[i];

			if (Utils::IsDepthFormat(attachment.Format))
			{
				depthAttachmentReference.attachment = i;

				if (Utils::IsStencilFormat(attachment.Format))
					depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				else
					depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			}
			else
			{
				auto& colorAttachmentReference = colorAttachmentReferences.emplace_back();
				colorAttachmentReference.attachment = i;
				colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
		}

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		if (!m_ColorAttachments.empty())
		{
			subpassDescription.colorAttachmentCount = static_cast<uint32>(colorAttachmentReferences.size());
			subpassDescription.pColorAttachments = colorAttachmentReferences.data();
		}

		if (m_DepthAttachment)
		{
			subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
		}

		std::array<VkSubpassDependency, 2> subpassDependencies;

		subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependencies[0].dstSubpass = 0;
		subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		subpassDependencies[1].srcSubpass = 0;
		subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = static_cast<uint32>(attachmentDescriptions.size());
		renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = static_cast<uint32>(subpassDependencies.size());
		renderPassCreateInfo.pDependencies = subpassDependencies.data();

		if (m_RenderPass)
			vkDestroyRenderPass(device, m_RenderPass, nullptr);

		VK_CHECK(vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &m_RenderPass));

		std::vector<VkImageView> attachments;

		for (uint32 i = 0; i < static_cast<uint32>(m_ColorAttachments.size()); i++)
			attachments.push_back(m_ColorAttachments[i].As<VulkanImage2D>()->GetAttachmentView());

		if (m_DepthAttachment)
			attachments.push_back(m_DepthAttachment.As<VulkanImage2D>()->GetAttachmentView());

		uint32 maxLayers = 0;
		for (uint32 i = 0; i < static_cast<uint32>(m_ColorAttachments.size()); i++)
		{
			uint32 layerCount = m_ColorAttachments[i]->GetCreateInfo().ArrayLayers;
			if (layerCount > maxLayers)
				maxLayers = layerCount;
		}

		if (m_DepthAttachment)
		{
			uint32 layerCount = m_DepthAttachment->GetCreateInfo().ArrayLayers;
			if (layerCount > maxLayers)
				maxLayers = layerCount;
		}

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = m_RenderPass;
		framebufferCreateInfo.attachmentCount = static_cast<uint32>(attachments.size());
		framebufferCreateInfo.pAttachments = attachments.data();
		framebufferCreateInfo.width = m_Width;
		framebufferCreateInfo.height = m_Height;
		framebufferCreateInfo.layers = maxLayers;

		if (m_Framebuffer)
			vkDestroyFramebuffer(device, m_Framebuffer, nullptr);

		VK_CHECK(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &m_Framebuffer));
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
			renderPassBeginInfo.renderArea.extent.width = m_Width;
			renderPassBeginInfo.renderArea.extent.height = m_Height;
			renderPassBeginInfo.renderPass = m_RenderPass;
			renderPassBeginInfo.framebuffer = m_Framebuffer;

			VkViewport viewport;
			viewport.x = 0.0f;
			viewport.y = (float)m_Height;
			viewport.width = (float)m_Width;
			viewport.height = -(float)m_Height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(activeCommandBuffer, 0, 1, &viewport);

			VkRect2D scissor;
			scissor.offset = { 0, 0 };
			scissor.extent = { m_Width, m_Height };
			vkCmdSetScissor(activeCommandBuffer, 0, 1, &scissor);
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