#include "FluxPCH.h"
#include "IndexBuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanIndexBuffer.h"
#endif

namespace Flux {

	Ref<IndexBuffer> IndexBuffer::Create(const void* data, uint32 size)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanIndexBuffer>::Create(data, size);
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}