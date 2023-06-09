#include "FluxPCH.h"
#include "GraphicsDevice.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Vulkan/VulkanDevice.h"

namespace Flux {

	Ref<GraphicsAdapter> GraphicsAdapter::Create(Ref<GraphicsContext> context)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanAdapter>::Create(context);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

	Ref<GraphicsDevice> GraphicsDevice::Create(Ref<GraphicsAdapter> adapter)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanDevice>::Create(adapter);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}