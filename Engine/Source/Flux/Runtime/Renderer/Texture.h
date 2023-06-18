#pragma once

#include "Image.h"
#include "Sampler.h"

namespace Flux {

	enum class TextureFilterMode : uint8
	{
		Point,
		Bilinear,
		Trilinear
	};

	enum class TextureWrapMode : uint8
	{
		Repeat,
		Clamp
	};

	struct TextureProperties
	{
		TextureFilterMode FilterMode = TextureFilterMode::Bilinear;
		TextureWrapMode WrapMode = TextureWrapMode::Repeat;
		bool GenerateMipmaps = true;
		uint32 MaxMipResolution = 1;
		uint32 AnisoLevel = 1;

		std::string DebugLabel = "Texture";
	};

	class Texture : public ReferenceCounted
	{
	public:
		virtual ~Texture() {}

		virtual Ref<Image> GetImage() const = 0;
		virtual Ref<Sampler> GetSampler() const = 0;

		virtual TextureProperties& GetProperties() = 0;
		virtual const TextureProperties& GetProperties() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D(const std::filesystem::path& path, const TextureProperties& properties = {});
		Texture2D(uint32 width, uint32 height, PixelFormat format = PixelFormat::RGBA, const void* data = nullptr, const TextureProperties& properties = {});

		virtual Ref<Image> GetImage() const override { return m_Image; }
		virtual Ref<Sampler> GetSampler() const { return m_Sampler; }

		virtual TextureProperties& GetProperties() { return m_Properties; }
		virtual const TextureProperties& GetProperties() const { return m_Properties; }
	private:
		void CreateImage();
		void CreateSampler();
	private:
		TextureProperties m_Properties;

		Ref<Image> m_Image;
		Ref<Sampler> m_Sampler;

		int32 m_Width = 0;
		int32 m_Height = 0;
		uint32 m_MipCount = 1;
		PixelFormat m_Format = PixelFormat::None;

		uint8* m_ImageBuffer = nullptr;
	};

}