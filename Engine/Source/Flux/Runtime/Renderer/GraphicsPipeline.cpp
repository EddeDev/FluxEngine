#include "FluxPCH.h"
#include "GraphicsPipeline.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanPipeline.h"
#endif

namespace Flux {

	Ref<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanPipeline>::Create(createInfo);
#endif
		default:
			FLUX_VERIFY(false, "Unknown Graphics API.");
			return nullptr;
		}
	}

}