#include "FluxPCH.h"
#include "Image.h"

#include "Flux/Runtime/Engine/Engine.h"

#include "Vulkan/VulkanImage.h"

namespace Flux {

	Ref<Image2D> Image2D::Create(const ImageCreateInfo& createInfo)
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanImage2D>::Create(createInfo);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}