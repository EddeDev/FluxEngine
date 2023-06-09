#include "FluxPCH.h"
#include "Framebuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Vulkan/VulkanFramebuffer.h"

namespace Flux {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferCreateInfo& createInfo)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanFramebuffer>::Create(createInfo);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}