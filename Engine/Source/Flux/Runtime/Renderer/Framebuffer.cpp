#include "FluxPCH.h"
#include "Framebuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanFramebuffer.h"
#endif

namespace Flux {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanFramebuffer>::Create(createInfo);
#endif
		default:
			FLUX_VERIFY(false, "Unknown Graphics API.");
			return nullptr;
		}
	}

}