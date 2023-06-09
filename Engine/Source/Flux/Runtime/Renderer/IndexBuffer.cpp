#include "FluxPCH.h"
#include "IndexBuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Vulkan/VulkanIndexBuffer.h"

namespace Flux {

	Ref<IndexBuffer> IndexBuffer::Create(const void* data, uint32 size)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanIndexBuffer>::Create(data, size);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}