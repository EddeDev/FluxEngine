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
			case TextureFormat::R8:              return GL_R;
			case TextureFormat::RG16:            return GL_RG;
			case TextureFormat::RGB24:           return GL_RGB;
			case TextureFormat::RGBA32:          return GL_RGBA;
			case TextureFormat::RFloat:          return GL_R;
			case TextureFormat::RGFloat:         return GL_RG;
			case TextureFormat::RGBFloat:        return GL_RGB;
			case TextureFormat::RGBAFloat:       return GL_RGBA;
			case TextureFormat::Depth24Stencil8: return GL_DEPTH_STENCIL;
			}
			FLUX_VERIFY(false, "Unknown texture format!");
			return 0;
		}

		static uint32 OpenGLInternalTextureFormat(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::R8:              return GL_R8;
			case TextureFormat::RG16:            return GL_RG8;
			case TextureFormat::RGB24:           return GL_RGB8;
			case TextureFormat::RGBA32:          return GL_RGBA8;
			case TextureFormat::RFloat:          return GL_R32F;
			case TextureFormat::RGFloat:         return GL_RG32F;
			case TextureFormat::RGBFloat:        return GL_RGB32F;
			case TextureFormat::RGBAFloat:       return GL_RGBA32F;
			case TextureFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
			}
			FLUX_VERIFY(false, "Unknown texture format!");
			return 0;
		}

		static uint32 OpenGLTextureDataType(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::R8:              return GL_UNSIGNED_BYTE;
			case TextureFormat::RG16:            return GL_UNSIGNED_BYTE;
			case TextureFormat::RGB24:           return GL_UNSIGNED_BYTE;
			case TextureFormat::RGBA32:          return GL_UNSIGNED_BYTE;
			case TextureFormat::RFloat:          return GL_FLOAT;
			case TextureFormat::RGFloat:         return GL_FLOAT;
			case TextureFormat::RGBFloat:        return GL_FLOAT;
			case TextureFormat::RGBAFloat:       return GL_FLOAT;
			case TextureFormat::Depth24Stencil8: return GL_UNSIGNED_INT_24_8;
			}
			FLUX_VERIFY(false, "Unknown texture format!");
			return 0;
		}

		static uint32 GetTextureTarget(const TextureProperties& properties)
		{
			if (properties.Samples > 1)
			{
				if (properties.Layers > 1)
					return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
				else
					return GL_TEXTURE_2D_MULTISAMPLE;
			}
			else
			{
				if (properties.Layers > 1)
					return GL_TEXTURE_2D_ARRAY;
				else
					return GL_TEXTURE_2D;
			}

			FLUX_VERIFY(false);
			return 0;
		}

	}

	OpenGLTexture::OpenGLTexture(const TextureProperties& properties, const void* data)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Data = new OpenGLTextureData();

		Reinitialize(properties);

		if (data)
		{
			SetData(data, properties.Width * properties.Height);
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

	void OpenGLTexture::Reinitialize(const TextureProperties& properties)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();
		
		FLUX_VERIFY(properties.IsValid());
		m_Properties = properties;

		if (properties.Usage == TextureUsage::Texture)
		{
			m_LocalStorage.Allocate(properties.Width * properties.Height * Utils::GetTextureFormatBPP(properties.Format));
			m_LocalStorage.FillWithZeros();
		}

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, properties = m_Properties]() mutable
		{
			if (data->TextureID)
				glDeleteTextures(1, &data->TextureID);

			data->TextureTarget = Utils::GetTextureTarget(properties);
			data->Format = Utils::OpenGLTextureFormat(properties.Format);
			data->InternalFormat = Utils::OpenGLInternalTextureFormat(properties.Format);
			data->DataType = Utils::OpenGLTextureDataType(properties.Format);

			glCreateTextures(data->TextureTarget, 1, &data->TextureID);

			if (properties.Layers > 1)
			{
				if (properties.Samples > 1)
					glTextureStorage3DMultisample(data->TextureID, properties.Samples, data->InternalFormat, properties.Width, properties.Height, properties.Layers, GL_FALSE);
				else
					glTextureStorage3D(data->TextureID, properties.MipCount, data->InternalFormat, properties.Width, properties.Height, properties.Layers);
			}
			else
			{
				if (properties.Samples > 1)
					glTextureStorage2DMultisample(data->TextureID, properties.Samples, data->InternalFormat, properties.Width, properties.Height, GL_FALSE);
				else
					glTextureStorage2D(data->TextureID, properties.MipCount, data->InternalFormat, properties.Width, properties.Height);
			}

			if (properties.Usage == TextureUsage::Texture)
			{
				glTextureParameteri(data->TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTextureParameteri(data->TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTextureParameteri(data->TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(data->TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}
		});
	}

	void OpenGLTexture::Apply()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		uint32 bufferIndex = m_Data->Storage.SetData(m_LocalStorage);

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, bufferIndex, properties = m_Properties]() mutable
		{
			Buffer buffer = data->Storage.GetBuffer(bufferIndex);

			if (properties.Layers > 1)
			{
				uint32 bytesPerPixel = Utils::GetTextureFormatBPP(properties.Format);

				uint32 offset = 0;
				for (uint32 layer = 0; layer < properties.Layers; layer++)
				{
					for (uint32 mip = 0; mip < properties.MipCount; mip++)
					{
						auto [width, height] = Utils::ComputeTextureMipSize(properties.Width, properties.Height, mip);
						glTextureSubImage3D(data->TextureID, mip, 0, 0, 0, width, height, layer, data->Format, data->DataType, buffer.GetData(offset));
						offset += width * height * bytesPerPixel;
					}
				}
			}
			else
			{
				glTextureSubImage2D(data->TextureID, 0, 0, 0, properties.Width, properties.Height, data->Format, data->DataType, buffer.Data);
			}

			data->Storage.SetBufferAvailable(bufferIndex);

			if (properties.MipCount > 1)
				glGenerateTextureMipmap(data->TextureID);
		});
	}

	void OpenGLTexture::AttachToFramebuffer(uint32 attachmentIndex)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, attachmentIndex, format = m_Properties.Format]()
		{
			uint32 attachment = Utils::IsDepthFormat(format) ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0 + attachmentIndex;
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, data->TextureTarget, data->TextureID, 0);
		});
	}

	void OpenGLTexture::AttachToFramebufferLayer(uint32 attachmentIndex, uint32 layer)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([data = m_Data, attachmentIndex, layer, format = m_Properties.Format]()
		{
			uint32 attachment = Utils::IsDepthFormat(format) ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0 + attachmentIndex;
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, data->TextureID, 0, layer);
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

		FLUX_VERIFY(m_Properties.Usage == TextureUsage::Texture);
		uint32 index = (y * m_Properties.Width + x);
		FLUX_VERIFY(index < m_Properties.Width * m_Properties.Height);
		uint32 bytesPerPixel = Utils::GetTextureFormatBPP(m_Properties.Format);
		m_LocalStorage.SetData(&value, bytesPerPixel, index * bytesPerPixel);
	}

	void OpenGLTexture::SetData(const void* data, uint32 count)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_VERIFY(m_Properties.Usage == TextureUsage::Texture);
		uint32 bytesPerPixel = Utils::GetTextureFormatBPP(m_Properties.Format);
		m_LocalStorage.SetData(data, count * bytesPerPixel);
	}

}
