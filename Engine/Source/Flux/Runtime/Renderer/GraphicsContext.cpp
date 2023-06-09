#include "FluxPCH.h"
#include "GraphicsContext.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Vulkan/VulkanContext.h"

namespace Flux {

	Ref<GraphicsContext> GraphicsContext::Create()
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanContext>::Create();
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}