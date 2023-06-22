#include "FluxPCH.h"
#include "Sampler.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Vulkan/VulkanSampler.h"

namespace Flux {

	Ref<Sampler> Sampler::Create(const SamplerCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanSampler>::Create(createInfo);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}