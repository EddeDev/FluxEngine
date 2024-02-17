#pragma once

#include "Flux/Runtime/Renderer/Framebuffer.h"

namespace Flux {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferCreateInfo& createInfo);
		virtual ~OpenGLFramebuffer();

		virtual void Invalidate() override;
		virtual void Resize(uint32 width, uint32 height) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual Ref<Texture> GetColorAttachment(uint32 index = 0) const override { return m_ColorAttachments[index]; }
		virtual Ref<Texture> GetDepthAttachment() const { return m_DepthAttachment; }
	private:
		std::vector<Ref<Texture>> m_ColorAttachments;
		Ref<Texture> m_DepthAttachment;

		uint32 m_Width = 0;
		uint32 m_Height = 0;

		struct OpenGLFramebufferData
		{
			FramebufferCreateInfo CreateInfo;
			uint32 FramebufferID;
		};

		OpenGLFramebufferData* m_Data = nullptr;
	};

}