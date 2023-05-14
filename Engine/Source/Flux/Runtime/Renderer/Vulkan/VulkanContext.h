#pragma once

#include "Flux/Runtime/Renderer/GraphicsContext.h"

#include "Vulkan.h"

namespace Flux {

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext();
		virtual ~VulkanContext();

		bool IsExtensionSupported(const char* extension) const
		{
			return std::find(m_SupportedInstanceExtensions.begin(), m_SupportedInstanceExtensions.end(), extension) != m_SupportedInstanceExtensions.end();
		}

		VkInstance GetInstance() const { return m_Instance; }
	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugUtilsMessenger = VK_NULL_HANDLE;

		std::vector<std::string> m_SupportedInstanceExtensions;
	};

}