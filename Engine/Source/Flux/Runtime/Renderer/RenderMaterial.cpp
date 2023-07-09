#include "FluxPCH.h"
#include "RenderMaterial.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanMaterial.h"
#endif

namespace Flux {

	Ref<RenderMaterial> RenderMaterial::Create(Ref<Shader> shader, const std::string& name)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanMaterial>::Create(shader, name);
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

	Ref<RenderMaterial> RenderMaterial::Create(Ref<RenderMaterial> other, const std::string& name)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanMaterial>::Create(other, name);
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}