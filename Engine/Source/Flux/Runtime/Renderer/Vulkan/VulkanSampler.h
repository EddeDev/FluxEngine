#pragma once

#include "Flux/Runtime/Renderer/Sampler.h"

#include "Vulkan.h"

namespace Flux {

	class VulkanSampler : public Sampler
	{
	public:
		VulkanSampler(const SamplerCreateInfo& createInfo);
		virtual ~VulkanSampler();

		virtual void Invalidate() override;
		virtual void RT_Invalidate() override;

		virtual void Release() override;
		virtual void RT_Release() override;

		virtual SamplerCreateInfo& GetCreateInfo() { return m_CreateInfo; }
		virtual const SamplerCreateInfo& GetCreateInfo() const override { return m_CreateInfo; }

		VkSampler GetSampler() const { return m_Sampler; }
	private:
		SamplerCreateInfo m_CreateInfo;

		VkSampler m_Sampler = VK_NULL_HANDLE;
	};

}