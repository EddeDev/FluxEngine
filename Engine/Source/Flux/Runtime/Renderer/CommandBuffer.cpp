#include "FluxPCH.h"
#include "CommandBuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Vulkan/VulkanCommandBuffer.h"

namespace Flux {

	Ref<CommandBuffer> CommandBuffer::Create(const CommandBufferCreateInfo& createInfo)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanCommandBuffer>::Create(createInfo);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}