#include "FluxPCH.h"
#include "OpenGLFramebuffer.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferCreateInfo& createInfo)
		: m_Width(createInfo.Width), m_Height(createInfo.Height)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (m_Width == 0 || m_Height == 0)
		{
			m_Width = Engine::Get().GetMainWindow()->GetWidth();
			m_Height = Engine::Get().GetMainWindow()->GetHeight();
		}

		m_Data = new OpenGLFramebufferData();
		m_Data->CreateInfo = createInfo;

		for (const auto& attachment : createInfo.Attachments)
		{
			TextureProperties properties;
			properties.Width = m_Width;
			properties.Height = m_Height;
			properties.Format = TextureFormat::RGBA32;

			Ref<Texture> texture = Texture::Create(properties);

			if (Utils::IsDepthFormat(attachment.Format))
				m_DepthAttachment = texture;
			else
				m_ColorAttachments.emplace_back(texture);
		}

		Invalidate();
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

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_Data->CreateInfo.SwapchainTarget)
			return;

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data]() mutable
		{
			if (data->FramebufferID)
				glDeleteFramebuffers(1, &data->FramebufferID);

			glCreateFramebuffers(1, &data->FramebufferID);
		});
	}

	void OpenGLFramebuffer::Resize(uint32 width, uint32 height)
	{
		if (m_Width == width && m_Height == height)
			return;

		m_Width = width;
		m_Height = height;

		Invalidate();
	}

	void OpenGLFramebuffer::Bind() const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, width = m_Width, height = m_Height, hasColorAttachment = !m_ColorAttachments.empty(), hasDepthAttachment = (bool)m_DepthAttachment]()
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
			{
				glDisable(GL_SCISSOR_TEST);
				glClear(clearFlags);
				glEnable(GL_SCISSOR_TEST);
			}

			glViewport(0, 0, width, height);
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