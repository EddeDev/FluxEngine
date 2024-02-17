#pragma once

namespace Flux {

	enum class TextureFormat : uint8
	{
		None = 0,

		R8,
		RG16,
		RGB24,
		RGBA32,

		RFloat,
		RGFloat,
		RGBFloat,
		RGBAFloat
	};

	struct TextureCreateInfo
	{
		const void* InitialData = nullptr;

		TextureFormat Format = TextureFormat::RGBA32;

		uint32 Width = 1;
		uint32 Height = 1;
		uint32 Layers = 1;
		uint32 Mips = 1;
		uint32 Samples = 1;
	};

	class Texture : public ReferenceCounted
	{
	public:
		virtual ~Texture() {}

		virtual void Reinitialize(uint32 width, uint32 height, TextureFormat format) = 0;
		virtual void Apply() = 0;

		virtual void Bind(uint32 slot = 0) const = 0;
		virtual void Unbind(uint32 slot = 0) const = 0;

		virtual void SetPixel(uint32 x, uint32 y, uint32 value) = 0;
		virtual void SetData(const void* data, uint32 count) = 0;

		virtual uint32 GetWidth() const = 0;
		virtual uint32 GetHeight() const = 0;

		static Ref<Texture> Create(const TextureCreateInfo& createInfo);
	};

	namespace Utils {

		inline static uint32 GetTextureFormatBPP(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::R8:        return 1 * 1;
			case TextureFormat::RG16:      return 2 * 1;
			case TextureFormat::RGB24:     return 3 * 1;
			case TextureFormat::RGBA32:    return 4 * 1;
			case TextureFormat::RFloat:    return 1 * 4;
			case TextureFormat::RGFloat:   return 2 * 4;
			case TextureFormat::RGBFloat:  return 3 * 4;
			case TextureFormat::RGBAFloat: return 4 * 4;
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