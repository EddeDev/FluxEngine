#include "FluxPCH.h"
#include "Image.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanImage.h"
#endif

namespace Flux {

	Ref<Image2D> Image2D::Create(const ImageCreateInfo& createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanImage2D>::Create(createInfo);
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}