#include "FluxPCH.h"
#include "UniformBuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Vulkan/VulkanUniformBuffer.h"

namespace Flux {

	Ref<UniformBuffer> UniformBuffer::Create(const UniformBufferCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanUniformBuffer>::Create(createInfo);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}