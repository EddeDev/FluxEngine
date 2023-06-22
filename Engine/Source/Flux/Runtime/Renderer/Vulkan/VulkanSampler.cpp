#include "FluxPCH.h"
#include "VulkanSampler.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"
#include "VulkanCompareOp.h"

namespace Flux {

	namespace Utils {

		static VkFilter VulkanSamplerFilterMode(SamplerFilterMode mode)
		{
			switch (mode)
			{
			case SamplerFilterMode::Nearest: return VK_FILTER_NEAREST;
			case SamplerFilterMode::Linear:  return VK_FILTER_LINEAR;
			}
			FLUX_VERIFY(false);
			return static_cast<VkFilter>(0);
		}

		static VkSamplerMipmapMode VulkanSamplerMipmapMode(SamplerMipmapMode mode)
		{
			switch (mode)
			{
			case SamplerMipmapMode::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
			case SamplerMipmapMode::Linear:  return VK_SAMPLER_MIPMAP_MODE_LINEAR;
			}
			FLUX_VERIFY(false);
			return static_cast<VkSamplerMipmapMode>(0);
		}

		static VkSamplerAddressMode VulkanSamplerAddressMode(SamplerAddressMode mode)
		{
			switch (mode)
			{
			case SamplerAddressMode::Repeat:            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case SamplerAddressMode::MirroredRepeat:    return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case SamplerAddressMode::ClampToEdge:       return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case SamplerAddressMode::ClampToBorder:     return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			case SamplerAddressMode::MirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
			}
			FLUX_VERIFY(false);
			return static_cast<VkSamplerAddressMode>(0);
		}

		static VkBorderColor VulkanSamplerBorderColor(SamplerBorderColor color)
		{
			switch (color)
			{
			case SamplerBorderColor::TransparentBlack: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
			case SamplerBorderColor::OpaqueBlack: return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
			case SamplerBorderColor::OpaqueWhite: return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			}
			FLUX_VERIFY(false);
			return static_cast<VkBorderColor>(0);
		}
		
	}

	VulkanSampler::VulkanSampler(const SamplerCreateInfo& createInfo)
		: m_CreateInfo(createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Invalidate();
	}

	VulkanSampler::~VulkanSampler()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Release();
	}

	void VulkanSampler::Invalidate()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<VulkanSampler> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_Invalidate();
		});
	}

	void VulkanSampler::RT_Invalidate()
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		RT_Release();

		VkDevice device = VulkanDevice::Get()->GetDevice();

		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = Utils::VulkanSamplerFilterMode(m_CreateInfo.MagFilterMode);
		samplerCreateInfo.minFilter = Utils::VulkanSamplerFilterMode(m_CreateInfo.MinFilterMode);
		samplerCreateInfo.mipmapMode = Utils::VulkanSamplerMipmapMode(m_CreateInfo.MipmapMode);
		samplerCreateInfo.addressModeU = Utils::VulkanSamplerAddressMode(m_CreateInfo.AddressModeU);
		samplerCreateInfo.addressModeV = Utils::VulkanSamplerAddressMode(m_CreateInfo.AddressModeV);
		samplerCreateInfo.addressModeW = Utils::VulkanSamplerAddressMode(m_CreateInfo.AddressModeW);
		samplerCreateInfo.mipLodBias = m_CreateInfo.MipLodBias;
		samplerCreateInfo.anisotropyEnable = m_CreateInfo.AnisotropyEnable;
		samplerCreateInfo.maxAnisotropy = m_CreateInfo.MaxAnisotropy;
		samplerCreateInfo.compareEnable = m_CreateInfo.CompareEnable;
		samplerCreateInfo.compareOp = Utils::VulkanCompareOp(m_CreateInfo.TextureCompareOp);
		samplerCreateInfo.minLod = m_CreateInfo.MinLod;
		samplerCreateInfo.maxLod = m_CreateInfo.MaxLod;
		samplerCreateInfo.borderColor = Utils::VulkanSamplerBorderColor(m_CreateInfo.BorderColor);

		VK_CHECK(vkCreateSampler(device, &samplerCreateInfo, nullptr, &m_Sampler));
	}

	void VulkanSampler::Release()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([sampler = m_Sampler]()
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();
			vkDestroySampler(device, sampler, nullptr);
		});
	}

	void VulkanSampler::RT_Release()
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		if (m_Sampler)
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();
			vkDestroySampler(device, m_Sampler, nullptr);
		}
	}

}