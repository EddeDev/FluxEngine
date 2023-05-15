#include "FluxPCH.h"
#include "Swapchain.h"

#include "Flux/Runtime/Engine/Engine.h"

#include "Vulkan/VulkanSwapchain.h"

namespace Flux {

	Ref<Swapchain> Swapchain::Create(Window* window)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanSwapchain>::Create(window);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}