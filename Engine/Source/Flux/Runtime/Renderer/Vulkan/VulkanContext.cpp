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

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT  messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		LogVerbosity verbosity = LogVerbosity::Trace;
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    verbosity = LogVerbosity::Info;    break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: verbosity = LogVerbosity::Warning; break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   verbosity = LogVerbosity::Error;   break;
		}

		FLUX_LOG_CATEGORY(verbosity, "Vulkan", "{0}", pCallbackData->pMessage);
		if (verbosity == LogVerbosity::Error)
			FLUX_VERIFY(false);

		return VK_FALSE;
	}

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
				m_SupportedInstanceExtensions.push_back(extension.extensionName);
		}

		if (s_EnableValidationLayers)
		{
			if (std::find(m_SupportedInstanceExtensions.begin(), m_SupportedInstanceExtensions.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) != m_SupportedInstanceExtensions.end())
				enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		for (const char* extensionName : enabledInstanceExtensions)
		{
			if (std::find(m_SupportedInstanceExtensions.begin(), m_SupportedInstanceExtensions.end(), extensionName) == m_SupportedInstanceExtensions.end())
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

		if (s_EnableValidationLayers)
		{
			VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {};
			messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			messengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			messengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
			messengerCreateInfo.pfnUserCallback = DebugUtilsMessengerCallback;

			auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
			VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_Instance, &messengerCreateInfo, nullptr, &m_DebugUtilsMessenger));
		}
	}

	VulkanContext::~VulkanContext()
	{
		if (m_DebugUtilsMessenger != VK_NULL_HANDLE)
		{
			auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
			vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugUtilsMessenger, nullptr);
		}

		vkDestroyInstance(m_Instance, nullptr);
	}

}