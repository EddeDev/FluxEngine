#include "FluxPCH.h"
#include "Sampler.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanSampler.h"
#endif

namespace Flux {

	Ref<Sampler> Sampler::Create(const SamplerCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanSampler>::Create(createInfo);
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}