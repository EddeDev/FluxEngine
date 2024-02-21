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
		RGBAFloat,

		Depth24Stencil8
	};

	enum class TextureUsage : uint8
	{
		Texture = 0,
		Attachment
	};

	struct TextureProperties
	{
		TextureFormat Format = TextureFormat::RGBA32;
		TextureUsage Usage = TextureUsage::Texture;

		uint32 Width = 1;
		uint32 Height = 1;
		uint32 Layers = 1;
		uint32 MipCount = 1;
		uint32 Samples = 1;

		bool IsValid() const
		{
			return Format != TextureFormat::None && Width > 0 && Height > 0 && Layers > 0 && MipCount > 0 && Samples > 0;
		}
	};

	class Texture : public ReferenceCounted
	{
	public:
		virtual ~Texture() {}

		virtual void Reinitialize(const TextureProperties& properties) = 0;
		virtual void Apply() = 0;
		virtual void AttachToFramebuffer(uint32 attachmentIndex) = 0;
		virtual void AttachToFramebufferLayer(uint32 attachmentIndex, uint32 layer) = 0;

		virtual void Bind(uint32 slot = 0) const = 0;
		virtual void Unbind(uint32 slot = 0) const = 0;

		virtual void SetPixel(uint32 x, uint32 y, uint32 value) = 0;
		virtual void SetData(const void* data, uint32 count) = 0;

		virtual const TextureProperties& GetProperties() const = 0;

		static Ref<Texture> Create(const TextureProperties& properties, const void* data = nullptr);
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
			return format == TextureFormat::Depth24Stencil8;
		}

		inline static uint32 ComputeTextureMipCount(uint32 width, uint32 height)
		{
			return (uint32)Math::Floor(Math::Log2(Math::Max((float)width, (float)height))) + 1;
		}

		inline static std::pair<uint32, uint32> ComputeTextureMipSize(uint32 width, uint32 height, uint32 mip)
		{
			while (mip > 0)
			{
				width /= 2;
				height /= 2;
				mip--;
			}
			return { width, height };
		}

	}

}