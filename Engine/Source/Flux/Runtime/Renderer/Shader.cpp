#include "FluxPCH.h"
#include "Shader.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanShader.h"
#endif

namespace Flux {

	Ref<Shader> Shader::Create(const std::filesystem::path& path)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return Ref<VulkanShader>::Create(path);
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}