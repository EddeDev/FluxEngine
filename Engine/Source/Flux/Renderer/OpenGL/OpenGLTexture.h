#pragma once

#include "Flux/Renderer/Texture.h"

namespace Flux {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32 width, uint32 height, TextureFormat format);
		virtual ~OpenGLTexture2D();

		virtual void Bind(uint32 slot) const override;
		virtual void Unbind(uint32 slot) const override;

		virtual void SetPixelData(const void* data, uint32 count) override;
	private:
		uint32 m_Width;
		uint32 m_Height;
		TextureFormat m_Format;

		uint32 m_TextureID = 0;

		RenderThreadStorage m_Storage;
	};

}