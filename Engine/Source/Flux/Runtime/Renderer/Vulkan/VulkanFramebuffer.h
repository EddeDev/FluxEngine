#pragma once

#include "Flux/Runtime/Renderer/Framebuffer.h"

#include "Vulkan.h"

namespace Flux {

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferCreateInfo& createInfo);
		virtual ~VulkanFramebuffer();

		virtual void Resize(uint32 width, uint32 height, bool forceRecreate = false) override;

		virtual void Bind(Ref<CommandBuffer> commandBuffer) override;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer) override;

		virtual void Unbind(Ref<CommandBuffer> commandBuffer) override;
		virtual void RT_Unbind(Ref<CommandBuffer> commandBuffer) override;

		virtual Ref<Image2D> GetColorAttachment(uint32 index = 0) override { return m_ColorAttachments.at(index); }
		virtual Ref<Image2D> GetDepthAttachment() override { return m_DepthAttachment; }

		virtual uint32 GetWidth() override;
		virtual uint32 GetHeight() override;

		VkRenderPass GetRenderPass();
	private:
		void Invalidate();
		void RT_Invalidate();
	private:
		FramebufferCreateInfo m_CreateInfo;
		uint32 m_Width = 0;
		uint32 m_Height = 0;
		std::mutex m_Mutex;

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;

		std::vector<Ref<Image2D>> m_ColorAttachments;
		Ref<Image2D> m_DepthAttachment;
	};

}