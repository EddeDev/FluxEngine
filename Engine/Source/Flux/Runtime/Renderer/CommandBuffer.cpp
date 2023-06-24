#include "FluxPCH.h"
#include "CommandBuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanCommandBuffer.h"
#endif

namespace Flux {

	Ref<CommandBuffer> CommandBuffer::Create(const CommandBufferCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanCommandBuffer>::Create(createInfo);
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}