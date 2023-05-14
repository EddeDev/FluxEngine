#include "FluxPCH.h"
#include "VulkanContext.h"

#ifdef FLUX_PLATFORM_WINDOWS
	#include <vulkan/vulkan_win32.h>
#endif

namespace Flux {

#ifdef FLUX_BUILD_DEBUG
	static bool s_EnableValidationLayers = true;
#else
	static bool s_EnableValidationLayers = false;
#endif

	VulkanContext::VulkanContext()
	{
		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = "Flux Engine";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "Flux Engine";
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion = VK_VERSION_1_3;

		std::vector<const char*> enabledInstanceLayers;

		uint32 layerCount;
		VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
		if (layerCount > 0)
		{
			std::vector<VkLayerProperties> layers(layerCount);
			VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, layers.data()));

			if (s_EnableValidationLayers)
			{
				const std::vector<const char*> validationLayers = {
					"VK_LAYER_KHRONOS_validation"
				};

				for (const char* layerName : validationLayers)
				{
					bool layerFound = false;

					for (const auto& layerProperties : layers)
					{
						if (strcmp(layerName, layerProperties.layerName) == 0)
						{
							layerFound = true;
							break;
						}
					}

					if (layerFound)
						enabledInstanceLayers.push_back(layerName);
				}
			}
		}

		FLUX_TRACE("Enabled instance layers ({0}):", enabledInstanceLayers.size());
		for (const char* layerName : enabledInstanceLayers)
			FLUX_TRACE("  {0}", layerName);

		std::vector<const char*> enabledInstanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };
#ifdef FLUX_PLATFORM_WINDOWS
		enabledInstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

		uint32 extensionCount;
		VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
		if (extensionCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extensionCount);
			VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()));

			for (const auto& extension : extensions)
				m_SupportedInstanceExtension.push_back(extension.extensionName);
		}

		if (s_EnableValidationLayers)
		{
			if (std::find(m_SupportedInstanceExtension.begin(), m_SupportedInstanceExtension.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) != m_SupportedInstanceExtension.end())
				enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		for (const char* extensionName : enabledInstanceExtensions)
		{
			if (std::find(m_SupportedInstanceExtension.begin(), m_SupportedInstanceExtension.end(), extensionName) == m_SupportedInstanceExtension.end())
				FLUX_VERIFY(false, "Requested instance extension '{0}' is not supported.", extensionName);
		}

		FLUX_TRACE("Enabled instance extensions ({0}):", enabledInstanceExtensions.size());
		for (const char* extensionName : enabledInstanceExtensions)
			FLUX_TRACE("  {0}", extensionName);

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledLayerCount = static_cast<uint32>(enabledInstanceLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = enabledInstanceLayers.data();
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32>(enabledInstanceExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensions.data();

		VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance));
	}

	VulkanContext::~VulkanContext()
	{
		vkDestroyInstance(m_Instance, nullptr);
	}

}