#include "FluxPCH.h"
#include "Texture.h"

#include <stb_image.h>

namespace Flux {

	Texture2D::Texture2D(const std::filesystem::path& path, const TextureProperties& properties)
		: m_Properties(properties)
	{
		std::string pathString = path.string();
		
		uint8* data = nullptr;
		if (stbi_is_hdr(pathString.c_str()))
		{
			data = (uint8*)stbi_loadf(pathString.c_str(), &m_Width, &m_Height, nullptr, STBI_rgb_alpha);
			m_Format = PixelFormat::RGBA32F;
		}
		else
		{
			data = stbi_load(pathString.c_str(), &m_Width, &m_Height, nullptr, STBI_rgb_alpha);
			m_Format = PixelFormat::RGBA;
		}

		if (!data)
		{
			FLUX_ERROR("Failed to load texture file: {0}\nSTBI: {1}", pathString, stbi_failure_reason());
			return;
		}

		uint32 dataSize = m_Width * m_Height * Utils::ComputeBytesPerPixel(m_Format);
		m_ImageBuffer = new uint8[dataSize];
		memcpy(m_ImageBuffer, data, dataSize);

		stbi_image_free(data);

		if (properties.GenerateMipmaps)
			m_MipCount = Utils::CalculateMipCount(m_Width, m_Height);

		CreateImage();
		CreateSampler();
	}

	Texture2D::Texture2D(uint32 width, uint32 height, PixelFormat format, const void* data, const TextureProperties& properties)
		: m_Width(width), m_Height(height), m_Format(format), m_Properties(properties)
	{
		if (data)
		{
			uint32 dataSize = m_Width * m_Height * Utils::ComputeBytesPerPixel(m_Format);
			m_ImageBuffer = new uint8[dataSize];
			memcpy(m_ImageBuffer, data, dataSize);
		}

		if (properties.GenerateMipmaps)
			m_MipCount = Utils::CalculateMipCount(m_Width, m_Height);

		CreateImage();
		CreateSampler();
	}

	void Texture2D::CreateImage()
	{
		ImageCreateInfo imageCreateInfo;
		imageCreateInfo.InitialData = m_ImageBuffer;
		imageCreateInfo.Format = m_Format;
		imageCreateInfo.Usage = m_ImageBuffer ? ImageUsage::Texture : ImageUsage::Storage;
		imageCreateInfo.Width = m_Width;
		imageCreateInfo.Height = m_Height;
		imageCreateInfo.MipLevels = m_MipCount;
		imageCreateInfo.DebugLabel = fmt::format("{0}-Image", m_Properties.DebugLabel);
		m_Image = Image2D::Create(imageCreateInfo);
		m_Image->Invalidate();
	}

	void Texture2D::CreateSampler()
	{
		SamplerCreateInfo samplerCreateInfo;

		if (m_Properties.FilterMode == TextureFilterMode::Point)
		{
			samplerCreateInfo.MinFilterMode = SamplerFilterMode::Nearest;
			samplerCreateInfo.MagFilterMode = SamplerFilterMode::Nearest;
			samplerCreateInfo.MipmapMode = SamplerMipmapMode::Linear;
		}

		if (m_Properties.FilterMode == TextureFilterMode::Bilinear)
		{
			samplerCreateInfo.MinFilterMode = SamplerFilterMode::Linear;
			samplerCreateInfo.MagFilterMode = SamplerFilterMode::Linear;
			samplerCreateInfo.MipmapMode = SamplerMipmapMode::Nearest;
		}

		if (m_Properties.FilterMode == TextureFilterMode::Trilinear)
		{
			samplerCreateInfo.MinFilterMode = SamplerFilterMode::Linear;
			samplerCreateInfo.MagFilterMode = SamplerFilterMode::Linear;
			samplerCreateInfo.MipmapMode = SamplerMipmapMode::Linear;
		}

		if (m_Properties.WrapMode == TextureWrapMode::Repeat)
		{
			samplerCreateInfo.AddressModeU = SamplerAddressMode::Repeat;
			samplerCreateInfo.AddressModeV = SamplerAddressMode::Repeat;
			samplerCreateInfo.AddressModeW = SamplerAddressMode::Repeat;
		}

		if (m_Properties.WrapMode == TextureWrapMode::Clamp)
		{
			samplerCreateInfo.AddressModeU = SamplerAddressMode::ClampToEdge;
			samplerCreateInfo.AddressModeV = SamplerAddressMode::ClampToEdge;
			samplerCreateInfo.AddressModeW = SamplerAddressMode::ClampToEdge;
		}

		if (m_Properties.MaxMipResolution > 1)
		{
			uint32 count = Utils::CalculateMipCount(m_Properties.MaxMipResolution, m_Properties.MaxMipResolution);
			samplerCreateInfo.MaxLod = (float)m_MipCount - (float)count;
		}
		else
		{
			samplerCreateInfo.MaxLod = (float)m_MipCount;
		}

		samplerCreateInfo.MipLodBias = 0.0f;
		samplerCreateInfo.CompareEnable = false;
		samplerCreateInfo.TextureCompareOp = CompareOp::Never;
		samplerCreateInfo.MinLod = 0.0f;
		samplerCreateInfo.MaxAnisotropy = (float)m_Properties.AnisoLevel;
		samplerCreateInfo.AnisotropyEnable = false;
		samplerCreateInfo.BorderColor = SamplerBorderColor::OpaqueWhite;
		samplerCreateInfo.DebugLabel = fmt::format("{0}-Sampler", m_Properties.DebugLabel);
	
		m_Sampler = Sampler::Create(samplerCreateInfo);
	}

}