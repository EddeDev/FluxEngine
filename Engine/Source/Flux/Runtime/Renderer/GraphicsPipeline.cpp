#include "FluxPCH.h"
#include "GraphicsPipeline.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Vulkan/VulkanPipeline.h"

namespace Flux {

	Ref<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanPipeline>::Create(createInfo);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}