#include "FluxPCH.h"
#include "GraphicsDevice.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanDevice.h"
#endif

namespace Flux {

	Ref<GraphicsAdapter> GraphicsAdapter::Create(Ref<GraphicsContext> context)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanAdapter>::Create(context);
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

	Ref<GraphicsDevice> GraphicsDevice::Create(Ref<GraphicsAdapter> adapter)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanDevice>::Create(adapter);
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}