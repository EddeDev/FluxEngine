#include "FluxPCH.h"
#include "GraphicsPipeline.h"

#include "Flux/Runtime/Engine/Engine.h"

#include "Vulkan/VulkanPipeline.h"

namespace Flux {

	Ref<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineCreateInfo& createInfo)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanPipeline>::Create(createInfo);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}