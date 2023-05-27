#include "FluxPCH.h"
#include "VertexBuffer.h"

#include "Flux/Runtime/Engine/Engine.h"

#include "Vulkan/VulkanVertexBuffer.h"

namespace Flux {

	Ref<VertexBuffer> VertexBuffer::Create(uint32 size)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanVertexBuffer>::Create(size);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32 size)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanVertexBuffer>::Create(data, size);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}