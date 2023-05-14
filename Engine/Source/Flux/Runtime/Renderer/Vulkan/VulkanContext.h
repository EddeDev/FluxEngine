#pragma once

#include "Flux/Runtime/Renderer/GraphicsContext.h"

#include "Vulkan.h"

namespace Flux {

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext();
		virtual ~VulkanContext();
	private:
		VkInstance m_Instance = nullptr;

		std::vector<std::string> m_SupportedInstanceExtension;
	};

}