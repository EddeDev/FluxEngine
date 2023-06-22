#include "FluxPCH.h"
#include "Image.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Vulkan/VulkanImage.h"

namespace Flux {

	Ref<Image2D> Image2D::Create(const ImageCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanImage2D>::Create(createInfo);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}