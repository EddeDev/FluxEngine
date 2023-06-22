#include "FluxPCH.h"
#include "Shader.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Vulkan/VulkanShader.h"

namespace Flux {

	Ref<Shader> Shader::Create(const std::filesystem::path& path)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return Ref<VulkanShader>::Create(path);
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

}