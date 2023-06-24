#include "FluxPCH.h"
#include "UniformBuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanUniformBuffer.h"
#endif

namespace Flux {

	Ref<UniformBuffer> UniformBuffer::Create(const UniformBufferCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanUniformBuffer>::Create(createInfo);
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}