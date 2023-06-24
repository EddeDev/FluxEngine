#include "FluxPCH.h"
#include "VertexBuffer.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanVertexBuffer.h"
#endif

namespace Flux {

	Ref<VertexBuffer> VertexBuffer::Create(uint32 size)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanVertexBuffer>::Create(size);
#endif
		default:
			FLUX_VERIFY(false, "Unknown Graphics API.");
			return nullptr;
		}
	}

	Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32 size)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanVertexBuffer>::Create(data, size);
#endif
		default:
			FLUX_VERIFY(false, "Unknown Graphics API.");
			return nullptr;
		}
	}

}