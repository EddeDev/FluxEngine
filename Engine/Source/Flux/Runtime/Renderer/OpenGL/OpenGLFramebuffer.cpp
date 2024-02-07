#include "FluxPCH.h"
#include "OpenGLFramebuffer.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Data = new OpenGLFramebufferData();
		m_Data->CreateInfo = createInfo;

		m_Data->Width = createInfo.Width;
		m_Data->Height = createInfo.Height;

		if (m_Data->Width == 0 || m_Data->Height == 0)
		{
			m_Data->Width = Engine::Get().GetMainWindow()->GetWidth();
			m_Data->Height = Engine::Get().GetMainWindow()->GetHeight();
		}

		for (const auto& attachment : createInfo.Attachments)
		{
			Ref<Texture2D> texture = Texture2D::Create(m_Data->Width, m_Data->Height, attachment.Format);

			if (Utils::IsDepthFormat(attachment.Format))
				m_DepthAttachment = texture;
			else
				m_ColorAttachments.emplace_back(texture);
		}

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data]() mutable
		{
			glCreateFramebuffers(1, &data->FramebufferID);
		});
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([data = m_Data]()
		{
			if (data->FramebufferID)
				glDeleteFramebuffers(1, &data->FramebufferID);
			delete data;
		});
	}

	// Resize

	void OpenGLFramebuffer::Bind() const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, hasColorAttachment = !m_ColorAttachments.empty(), hasDepthAttachment = (bool)m_DepthAttachment]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, data->FramebufferID);

			const auto& createInfo = data->CreateInfo;

			uint32 clearFlags = 0;

			if ((hasColorAttachment || createInfo.SwapchainTarget) && createInfo.ClearColorBuffer)
			{
				const Vector4& clearColor = createInfo.ClearColor;
				glClearColor(clearColor.R, clearColor.G, clearColor.B, clearColor.A);

				clearFlags |= GL_COLOR_BUFFER_BIT;
			}

			if ((hasDepthAttachment || createInfo.SwapchainTarget) && createInfo.ClearDepthBuffer)
			{
				glDepthMask(GL_TRUE);
				glClearDepthf(createInfo.DepthClearValue);

				clearFlags |= GL_DEPTH_BUFFER_BIT;
			}

			if (clearFlags)
				glClear(clearFlags);

			glViewport(0, 0, data->Width, data->Height);
		});
	}

	void OpenGLFramebuffer::Unbind() const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, hasDepthAttachment = (bool)m_DepthAttachment]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			const auto& createInfo = data->CreateInfo;
			if ((hasDepthAttachment || createInfo.SwapchainTarget) && createInfo.ClearDepthBuffer)
				glDepthMask(GL_FALSE);
		});
	}

}