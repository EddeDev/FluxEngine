#pragma once

#include "Flux/Runtime/Renderer/Texture.h"

namespace Flux {

	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const TextureProperties& properties, const void* data);
		virtual ~OpenGLTexture();

		virtual void Reinitialize(const TextureProperties& properties) override;
		virtual void Apply() override;
		virtual void AttachToFramebuffer(uint32 attachmentIndex) override;
		virtual void AttachToFramebufferLayer(uint32 attachmentIndex, uint32 layer) override;

		virtual void Bind(uint32 slot) const override;
		virtual void Unbind(uint32 slot) const override;

		virtual void SetPixel(uint32 x, uint32 y, uint32 value) override;
		virtual void SetData(const void* data, uint32 count) override;

		virtual const TextureProperties& GetProperties() const { return m_Properties; }
	private:
		TextureProperties m_Properties;
		Buffer m_LocalStorage;

		struct OpenGLTextureData
		{
			RenderThreadStorage Storage;

			uint32 TextureID;
			uint32 TextureTarget;
			uint32 Format;
			uint32 InternalFormat;
			uint32 DataType;
		};

		OpenGLTextureData* m_Data = nullptr;
	};

}