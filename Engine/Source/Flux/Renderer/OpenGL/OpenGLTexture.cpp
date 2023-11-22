#include "FluxPCH.h"
#include "OpenGLTexture.h"

#include "Flux/Core/Engine.h"
#include "Flux/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

	namespace Utils {

		static uint32 OpenGLTextureFormat(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::RGBA32: return GL_RGBA;
			}
			FLUX_VERIFY(false, "Unknown texture format!");
			return 0;
		}

		static uint32 OpenGLInternalTextureFormat(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::RGBA32: return GL_RGBA8;
			}
			FLUX_VERIFY(false, "Unknown texture format!");
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(uint32 width, uint32 height, TextureFormat format)
		: m_Width(width), m_Height(height), m_Format(format)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Storage.SetSize(width * height * Utils::GetTextureFormatBPP(format));

		Ref<OpenGLTexture2D> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, width, height, format]() mutable
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_TextureID);

			glTextureParameteri(instance->m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(instance->m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// TODO: optional
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			glTextureStorage2D(instance->m_TextureID, 1, Utils::OpenGLInternalTextureFormat(format), width, height);
		});
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([textureID = m_TextureID]() mutable
		{
			if (textureID)
				glDeleteTextures(1, &textureID);
		});
	}

	void OpenGLTexture2D::Bind(uint32 slot) const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<const OpenGLTexture2D> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, slot]()
		{
			glBindTextureUnit(slot, instance->m_TextureID);
		});
	}

	void OpenGLTexture2D::Unbind(uint32 slot) const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([slot]()
		{
			glBindTextureUnit(slot, 0);
		});
	}

	void OpenGLTexture2D::SetPixelData(const void* data, uint32 count)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		uint32 bufferIndex = m_Storage.SetData(data, count * Utils::GetTextureFormatBPP(m_Format));

		Ref<OpenGLTexture2D> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, bufferIndex, width = m_Width, height = m_Height, format = m_Format]() mutable
		{
			Buffer buffer = instance->m_Storage.GetBuffer(bufferIndex);
			glTextureSubImage2D(instance->m_TextureID, 0, 0, 0, width, height, Utils::OpenGLTextureFormat(format), GL_UNSIGNED_BYTE, buffer.Data);
			instance->m_Storage.SetBufferAvailable(bufferIndex);
		});
	}

}