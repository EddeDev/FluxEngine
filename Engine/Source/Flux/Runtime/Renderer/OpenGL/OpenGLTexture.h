#pragma once

#include "Flux/Runtime/Renderer/Texture.h"

namespace Flux {

	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const TextureCreateInfo& createInfo);
		virtual ~OpenGLTexture();

		virtual void Reinitialize(uint32 width, uint32 height, TextureFormat format) override;
		virtual void Apply() override;

		virtual void Bind(uint32 slot) const override;
		virtual void Unbind(uint32 slot) const override;

		virtual void SetPixel(uint32 x, uint32 y, uint32 value) override;
		virtual void SetData(const void* data, uint32 count) override;

		virtual uint32 GetWidth() const override { return m_Width; }
		virtual uint32 GetHeight() const override { return m_Height; }
	private:
		uint32 m_Width = 0;
		uint32 m_Height = 0;
		TextureFormat m_Format = TextureFormat::None;

		Buffer m_LocalStorage;

		struct OpenGLTextureData
		{
			RenderThreadStorage Storage;
			uint32 TextureID;
		};

		OpenGLTextureData* m_Data = nullptr;
	};

}