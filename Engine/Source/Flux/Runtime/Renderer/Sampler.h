#pragma once

#include "CompareOp.h"

namespace Flux {

	enum class SamplerFilterMode : uint8
	{
		None = 0,
		Nearest,
		Linear
	};

	enum class SamplerMipmapMode : uint8
	{
		None = 0,
		Nearest,
		Linear
	};

	enum class SamplerAddressMode : uint8
	{
		None = 0,
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
		MirrorClampToEdge
	};

	enum class SamplerBorderColor : uint8
	{
		None = 0,
		TransparentBlack,
		OpaqueBlack,
		OpaqueWhite
	};

	struct SamplerCreateInfo
	{
		SamplerFilterMode MagFilterMode = SamplerFilterMode::Linear;
		SamplerFilterMode MinFilterMode = SamplerFilterMode::Linear;
		SamplerMipmapMode MipmapMode = SamplerMipmapMode::Linear;

		SamplerAddressMode AddressModeU = SamplerAddressMode::Repeat;
		SamplerAddressMode AddressModeV = SamplerAddressMode::Repeat;
		SamplerAddressMode AddressModeW = SamplerAddressMode::Repeat;

		float MipLodBias = 0.0f;

		bool AnisotropyEnable = false;
		float MaxAnisotropy = 1.0f;

		bool CompareEnable = false;
		CompareOp TextureCompareOp = CompareOp::Never;
		
		float MinLod = 0.0f;
		float MaxLod = 0.0f;

		SamplerBorderColor BorderColor = SamplerBorderColor::OpaqueWhite;

		std::string DebugLabel = "Sampler";
	};

	class Sampler : public ReferenceCounted
	{
	public:
		virtual ~Sampler() {}

		virtual void Invalidate() = 0;
		virtual void RT_Invalidate() = 0;

		virtual void Release() = 0;
		virtual void RT_Release() = 0;

		virtual SamplerCreateInfo& GetCreateInfo() = 0;
		virtual const SamplerCreateInfo& GetCreateInfo() const = 0;

		static Ref<Sampler> Create(const SamplerCreateInfo& createInfo);
	};

	namespace Utils {

		inline static glm::vec4 SamplerBorderColorValue(SamplerBorderColor borderColor)
		{
			switch (borderColor)
			{
			case SamplerBorderColor::TransparentBlack: return { 0.0f, 0.0f, 0.0f, 0.0f };
			case SamplerBorderColor::OpaqueBlack:      return { 0.0f, 0.0f, 0.0f, 1.0f };
			case SamplerBorderColor::OpaqueWhite:      return { 1.0f, 1.0f, 1.0f, 1.0f };
			}
			FLUX_VERIFY(false);
			return {};
		}

	}

}