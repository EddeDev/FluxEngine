#include "FluxPCH.h"
#include "OpenGLTexture.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

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

		m_Data = new OpenGLTextureData();
		m_Data->Storage.SetSize(width * height * Utils::GetTextureFormatBPP(format));

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, width, height, format]() mutable
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &data->TextureID);

			glTextureParameteri(data->TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(data->TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// TODO: optional
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			glTextureStorage2D(data->TextureID, 1, Utils::OpenGLInternalTextureFormat(format), width, height);
		});
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([data = m_Data]()
		{
			if (data->TextureID)
				glDeleteTextures(1, &data->TextureID);
			delete data;
		});
	}

	void OpenGLTexture2D::Bind(uint32 slot) const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, slot]()
		{
			glBindTextureUnit(slot, data->TextureID);
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

		uint32 bufferIndex = m_Data->Storage.SetData(data, count * Utils::GetTextureFormatBPP(m_Format));

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, bufferIndex, width = m_Width, height = m_Height, format = m_Format]() mutable
		{
			auto& buffer = data->Storage.GetBuffer(bufferIndex);
			glTextureSubImage2D(data->TextureID, 0, 0, 0, width, height, Utils::OpenGLTextureFormat(format), GL_UNSIGNED_BYTE, buffer.Data);
			data->Storage.SetBufferAvailable(bufferIndex);
		});
	}

}