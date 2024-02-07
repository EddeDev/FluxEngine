#pragma once

#include "Flux/Runtime/Renderer/Framebuffer.h"

namespace Flux {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferCreateInfo& createInfo);
		virtual ~OpenGLFramebuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual Ref<Texture2D> GetColorAttachment(uint32 index = 0) const override { return m_ColorAttachments[index]; }
		virtual Ref<Texture2D> GetDepthAttachment() const { return m_DepthAttachment; }
	private:
		std::vector<Ref<Texture2D>> m_ColorAttachments;
		Ref<Texture2D> m_DepthAttachment;

		struct OpenGLFramebufferData
		{
			FramebufferCreateInfo CreateInfo;
			uint32 Width;
			uint32 Height;
			uint32 FramebufferID;
		};

		OpenGLFramebufferData* m_Data = nullptr;
	};

}