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
			case TextureFormat::R8:        return GL_R;
			case TextureFormat::RG16:      return GL_RG;
			case TextureFormat::RGB24:     return GL_RGB;
			case TextureFormat::RGBA32:    return GL_RGBA;
			case TextureFormat::RFloat:    return GL_R;
			case TextureFormat::RGFloat:   return GL_RG;
			case TextureFormat::RGBFloat:  return GL_RGB;
			case TextureFormat::RGBAFloat: return GL_RGBA;
			}
			FLUX_VERIFY(false, "Unknown texture format!");
			return 0;
		}

		static uint32 OpenGLInternalTextureFormat(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::R8:        return GL_R8;
			case TextureFormat::RG16:      return GL_RG8;
			case TextureFormat::RGB24:     return GL_RGB8;
			case TextureFormat::RGBA32:    return GL_RGBA8;
			case TextureFormat::RFloat:    return GL_R32F;
			case TextureFormat::RGFloat:   return GL_RG32F;
			case TextureFormat::RGBFloat:  return GL_RGB32F;
			case TextureFormat::RGBAFloat: return GL_RGBA32F;
			}
			FLUX_VERIFY(false, "Unknown texture format!");
			return 0;
		}

		static uint32 OpenGLTextureDataType(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::R8:        return GL_UNSIGNED_BYTE;
			case TextureFormat::RG16:      return GL_UNSIGNED_BYTE;
			case TextureFormat::RGB24:     return GL_UNSIGNED_BYTE;
			case TextureFormat::RGBA32:    return GL_UNSIGNED_BYTE;
			case TextureFormat::RFloat:    return GL_FLOAT;
			case TextureFormat::RGFloat:   return GL_FLOAT;
			case TextureFormat::RGBFloat:  return GL_FLOAT;
			case TextureFormat::RGBAFloat: return GL_FLOAT;
			}
			FLUX_VERIFY(false, "Unknown texture format!");
			return 0;
		}

	}

	OpenGLTexture::OpenGLTexture(const TextureCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Data = new OpenGLTextureData();

		Reinitialize(createInfo.Width, createInfo.Height, createInfo.Format);

		if (createInfo.InitialData)
		{
			SetData(createInfo.InitialData, createInfo.Width * createInfo.Height);
			Apply();
		}
	}

	OpenGLTexture::~OpenGLTexture()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([data = m_Data]()
		{
			if (data->TextureID)
				glDeleteTextures(1, &data->TextureID);
			delete data;
		});

		m_LocalStorage.Release();
	}

	void OpenGLTexture::Reinitialize(uint32 width, uint32 height, TextureFormat format)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();
		
		m_Width = width;
		m_Height = height;
		m_Format = format;

		m_LocalStorage.Allocate(width * height * Utils::GetTextureFormatBPP(format));
		m_LocalStorage.FillWithZeros();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, width = m_Width, height = m_Height, format = m_Format]() mutable
		{
			if (data->TextureID)
				glDeleteTextures(1, &data->TextureID);

			glCreateTextures(GL_TEXTURE_2D, 1, &data->TextureID);

			glTextureParameteri(data->TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(data->TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureParameteri(data->TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(data->TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureStorage2D(data->TextureID, 1, Utils::OpenGLInternalTextureFormat(format), width, height);
		});
	}

	void OpenGLTexture::Apply()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		uint32 bufferIndex = m_Data->Storage.SetData(m_LocalStorage);

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, bufferIndex, width = m_Width, height = m_Height, format = m_Format]() mutable
		{
			auto& buffer = data->Storage.GetBuffer(bufferIndex);
			glTextureSubImage2D(data->TextureID, 0, 0, 0, width, height, Utils::OpenGLTextureFormat(format), Utils::OpenGLTextureDataType(format), buffer.Data);
			data->Storage.SetBufferAvailable(bufferIndex);
			glGenerateTextureMipmap(data->TextureID);
		});
	}

	void OpenGLTexture::Bind(uint32 slot) const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, slot]()
		{
			glBindTextureUnit(slot, data->TextureID);
		});
	}

	void OpenGLTexture::Unbind(uint32 slot) const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([slot]()
		{
			glBindTextureUnit(slot, 0);
		});
	}

	void OpenGLTexture::SetPixel(uint32 x, uint32 y, uint32 value)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		uint32 bytesPerPixel = Utils::GetTextureFormatBPP(m_Format);
		m_LocalStorage.SetData(&value, bytesPerPixel, (y * m_Width + x) * bytesPerPixel);
	}

	void OpenGLTexture::SetData(const void* data, uint32 count)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		uint32 bytesPerPixel = Utils::GetTextureFormatBPP(m_Format);
		m_LocalStorage.SetData(data, count * bytesPerPixel);
	}

}