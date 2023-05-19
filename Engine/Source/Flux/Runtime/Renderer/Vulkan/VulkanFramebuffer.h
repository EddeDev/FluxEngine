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

		virtual void Bind(Ref<CommandBuffer> commandBuffer) const override;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer) const override;

		virtual void Unbind(Ref<CommandBuffer> commandBuffer) const override;
		virtual void RT_Unbind(Ref<CommandBuffer> commandBuffer) const override;

		virtual uint32 GetWidth() const override
		{
			FLUX_ASSERT(!m_CreateInfo.SwapchainTarget);
			return m_Width;
		}

		virtual uint32 GetHeight() const override
		{
			FLUX_ASSERT(!m_CreateInfo.SwapchainTarget);
			return m_Height;
		}
	private:
		void Invalidate();
	private:
		FramebufferCreateInfo m_CreateInfo;
		uint32 m_Width = 0;
		uint32 m_Height = 0;
	};

}