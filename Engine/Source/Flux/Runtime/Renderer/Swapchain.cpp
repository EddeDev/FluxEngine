#include "FluxPCH.h"
#include "Swapchain.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanSwapchain.h"
#endif

namespace Flux {

	Ref<Swapchain> Swapchain::Create(Window* window)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanSwapchain>::Create(window);
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}