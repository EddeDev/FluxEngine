#include "FluxPCH.h"
#include "OpenGLFramebuffer.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

	namespace Utils {

		static uint32 OpenGLDepthCompareFunction(CompareFunction function)
		{
			switch (function)
			{
			case CompareFunction::Never:          return GL_NEVER;
			case CompareFunction::Less:           return GL_LESS;
			case CompareFunction::Equal:          return GL_EQUAL;
			case CompareFunction::LessOrEqual:    return GL_LEQUAL;
			case CompareFunction::Greater:        return GL_GREATER;
			case CompareFunction::NotEqual:       return GL_NOTEQUAL;
			case CompareFunction::GreaterOrEqual: return GL_GEQUAL;
			case CompareFunction::Always:         return GL_ALWAYS;
			}
			FLUX_VERIFY(false, "Unknown compare function!");
			return 0;
		}

		static const char* OpenGLFramebufferStatusToString(uint32 status)
		{
			switch (status)
			{
			case GL_FRAMEBUFFER_COMPLETE:                       return "GL_FRAMEBUFFER_COMPLETE";
			case GL_FRAMEBUFFER_UNDEFINED:                      return "GL_FRAMEBUFFER_UNDEFINED";
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:          return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:  return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			case GL_FRAMEBUFFER_UNSUPPORTED:                    return "GL_FRAMEBUFFER_UNSUPPORTED";
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:         return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:      return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT";
			}
			FLUX_VERIFY(false);
			return "";
		}

	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferCreateInfo& createInfo)
		: m_CreateInfo(createInfo), m_Width(createInfo.Width), m_Height(createInfo.Height)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (m_Width == 0 || m_Height == 0)
		{
			m_Width = Engine::Get().GetMainWindow()->GetWidth();
			m_Height = Engine::Get().GetMainWindow()->GetHeight();
		}

		m_Data = new OpenGLFramebufferData();

		for (const auto& attachment : createInfo.Attachments)
		{
			TextureProperties properties;
			properties.Width = m_Width;
			properties.Height = m_Height;
			properties.Format = attachment.Format;
			properties.Usage = TextureUsage::Attachment;

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
		if (m_CreateInfo.SwapchainTarget)
			return;

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, createInfo = m_CreateInfo]() mutable
		{
			if (data->FramebufferID)
				glDeleteFramebuffers(1, &data->FramebufferID);

			glCreateFramebuffers(1, &data->FramebufferID);
			glObjectLabel(GL_FRAMEBUFFER, data->FramebufferID, createInfo.DebugLabel.size(), createInfo.DebugLabel.c_str());
		
			glBindFramebuffer(GL_FRAMEBUFFER, data->FramebufferID);
		});

		uint32 attachmentIndex = 0;
		for (const auto& attachment : m_CreateInfo.Attachments)
		{
			if (Utils::IsDepthFormat(attachment.Format))
			{
				TextureProperties properties = m_DepthAttachment->GetProperties();
				if (properties.Width != m_Width || properties.Height != m_Height)
				{
					properties.Width = m_Width;
					properties.Height = m_Height;

					m_DepthAttachment->Reinitialize(properties);
				}

				m_DepthAttachment->AttachToFramebuffer(attachmentIndex);
			}
			else
			{
				TextureProperties properties = m_ColorAttachments[attachmentIndex]->GetProperties();
				if (properties.Width != m_Width || properties.Height != m_Height)
				{
					properties.Width = m_Width;
					properties.Height = m_Height;

					m_ColorAttachments[attachmentIndex]->Reinitialize(properties);
				}

				m_ColorAttachments[attachmentIndex]->AttachToFramebuffer(attachmentIndex);
			}

			attachmentIndex++;
		}

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, numColorAttachments = (uint32)m_ColorAttachments.size()]() mutable
		{
			if (numColorAttachments > 0)
			{
				std::vector<uint32> buffers(numColorAttachments);
				for (uint32 i = 0; i < numColorAttachments; i++)
					buffers[i] = GL_COLOR_ATTACHMENT0 + i;
				glDrawBuffers(buffers.size(), buffers.data());
			}
			else
			{
				glDrawBuffer(GL_NONE);
			}

			uint32 status = glCheckNamedFramebufferStatus(data->FramebufferID, GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE)
			{
				FLUX_ERROR_CATEGORY("OpenGL", "{0}", Utils::OpenGLFramebufferStatusToString(status));
				FLUX_VERIFY(false);
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, width = m_Width, height = m_Height, createInfo = m_CreateInfo, hasColorAttachment = !m_ColorAttachments.empty(), hasDepthAttachment = (bool)m_DepthAttachment]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, data->FramebufferID);

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
				glDepthFunc(Utils::OpenGLDepthCompareFunction(createInfo.DepthCompareFunction));
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

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, createInfo = m_CreateInfo, hasDepthAttachment = (bool)m_DepthAttachment]()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			if ((hasDepthAttachment || createInfo.SwapchainTarget) && createInfo.ClearDepthBuffer)
				glDepthMask(GL_FALSE);
		});
	}

}