#include "FluxPCH.h"
#include "GraphicsContext.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanContext.h"
#endif

namespace Flux {

	Ref<GraphicsContext> GraphicsContext::Create()
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanContext>::Create();
#endif
		default:
			FLUX_VERIFY(false, "Unknown Graphics API.");
			return nullptr;
		}
	}

}