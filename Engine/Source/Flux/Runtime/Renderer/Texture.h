#pragma once

namespace Flux {

	enum class TextureFormat : uint8
	{
		None = 0,

		RGBA32
	};

	class Texture : public ReferenceCounted
	{
	public:
		virtual ~Texture() {}

		virtual void Bind(uint32 slot = 0) const = 0;
		virtual void Unbind(uint32 slot = 0) const = 0;

		virtual void SetPixelData(const void* data, uint32 count) = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32 width, uint32 height, TextureFormat format = TextureFormat::RGBA32);
	};

	// TODO: RenderTexture?

	namespace Utils {

		inline static uint32 GetTextureFormatBPP(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::RGBA32: return 4;
			}
			FLUX_VERIFY(false, "Unknown texture format!");
			return 0;
		}

		inline static bool IsDepthFormat(TextureFormat format)
		{
			// TODO
			return false;
		}

	}

}