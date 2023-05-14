#include "FluxPCH.h"
#include "VulkanDevice.h"

#include "Vulkan.h"

namespace Flux {

	static const float s_DefaultQueuePriority = 0.0f;

	VulkanAdapter::VulkanAdapter(Ref<GraphicsContext> context)
		: m_Context(context.As<VulkanContext>())
	{
		uint32 physicalDeviceCount;
		VK_CHECK(vkEnumeratePhysicalDevices(m_Context->GetInstance(), &physicalDeviceCount, nullptr));
		FLUX_VERIFY(physicalDeviceCount > 0);

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		VK_CHECK(vkEnumeratePhysicalDevices(m_Context->GetInstance(), &physicalDeviceCount, physicalDevices.data()));

		size_t discreteDeviceIndex = std::numeric_limits<size_t>::max();

		for (size_t i = 0; i < physicalDevices.size(); i++)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				discreteDeviceIndex = i;
				break;
			}
		}

		if (discreteDeviceIndex != std::numeric_limits<size_t>::max())
			m_SelectedDevice = physicalDevices[discreteDeviceIndex];
		else
			m_SelectedDevice = physicalDevices.front();

		FLUX_VERIFY(m_SelectedDevice, "Failed to select physical device.");
		
		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties(m_SelectedDevice, nullptr, &extensionCount, nullptr);
		if (extensionCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extensionCount);
			VK_CHECK(vkEnumerateDeviceExtensionProperties(m_SelectedDevice, nullptr, &extensionCount, extensions.data()));

			m_SupportedExtensions.resize(extensionCount);
			for (uint32 i = 0; i < extensionCount; i++)
				m_SupportedExtensions[i] = extensions[i].extensionName;
		}

		uint32 queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(m_SelectedDevice, &queueFamilyCount, nullptr);
		FLUX_VERIFY(queueFamilyCount > 0);

		m_QueueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_SelectedDevice, &queueFamilyCount, m_QueueFamilyProperties.data());

		VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT;

		if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
		{
			m_QueueFamilyIndices.Graphics = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);

			auto& createInfo = m_QueueCreateInfos.emplace_back();
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.queueFamilyIndex = m_QueueFamilyIndices.Graphics;
			createInfo.queueCount = 1;
			createInfo.pQueuePriorities = &s_DefaultQueuePriority;
		}
		else
		{
			m_QueueFamilyIndices.Graphics = 0;
		}

		if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
		{
			m_QueueFamilyIndices.Compute = GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);

			if (m_QueueFamilyIndices.Compute != m_QueueFamilyIndices.Graphics)
			{
				auto& createInfo = m_QueueCreateInfos.emplace_back();
				createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				createInfo.queueFamilyIndex = m_QueueFamilyIndices.Compute;
				createInfo.queueCount = 1;
				createInfo.pQueuePriorities = &s_DefaultQueuePriority;
			}
		}
		else
		{
			m_QueueFamilyIndices.Compute = m_QueueFamilyIndices.Graphics;
		}

		if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
		{
			m_QueueFamilyIndices.Transfer = GetQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);

			if ((m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Graphics) && (m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Compute))
			{
				auto& createInfo = m_QueueCreateInfos.emplace_back();
				createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				createInfo.queueFamilyIndex = m_QueueFamilyIndices.Transfer;
				createInfo.queueCount = 1;
				createInfo.pQueuePriorities = &s_DefaultQueuePriority;
			}
		}
		else
		{
			m_QueueFamilyIndices.Transfer = m_QueueFamilyIndices.Graphics;
		}
	}

	uint32 VulkanAdapter::GetQueueFamilyIndex(VkQueueFlags queueFlags) const
	{
		if ((queueFlags & VK_QUEUE_COMPUTE_BIT) == queueFlags)
		{
			for (uint32 i = 0; i < static_cast<uint32>(m_QueueFamilyProperties.size()); i++)
			{
				if ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
					return i;
			}
		}

		if ((queueFlags & VK_QUEUE_TRANSFER_BIT) == queueFlags)
		{
			for (uint32 i = 0; i < static_cast<uint32>(m_QueueFamilyProperties.size()); i++)
			{
				if ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
					return i;
			}
		}

		for (uint32 i = 0; i < static_cast<uint32>(m_QueueFamilyProperties.size()); i++)
		{
			if ((m_QueueFamilyProperties[i].queueFlags & queueFlags) == queueFlags)
				return i;
		}

		FLUX_VERIFY(false, "Could not find a matching queue family index.");
		return 0;
	}

	VulkanDevice::VulkanDevice(Ref<GraphicsAdapter> adapter)
		: m_Adapter(adapter.As<VulkanAdapter>())
	{
		const auto& queueCreateInfos = m_Adapter->GetQueueCreateInfos();
		FLUX_VERIFY(!queueCreateInfos.empty());

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

		VK_CHECK(vkCreateDevice(m_Adapter->GetPhysicalDevice(), &deviceCreateInfo, nullptr, &m_Device));

		__debugbreak();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(m_Device, nullptr);
	}

}