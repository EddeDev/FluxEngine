#include "FluxPCH.h"
#include "VulkanDevice.h"

#include "Flux/Runtime/Core/Engine.h"

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
		FLUX_CHECK_IS_RENDER_THREAD();
		FLUX_VERIFY(!s_Instance);
		s_Instance = this;

		std::vector<const char*> enabledDeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkPhysicalDeviceFeatures enabledFeatures = {};
		enabledFeatures.samplerAnisotropy = VK_TRUE;

		const VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT;

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
		{
			m_QueueFamilyIndices.Graphics = m_Adapter->GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);

			auto& createInfo = queueCreateInfos.emplace_back();
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
			m_QueueFamilyIndices.Compute = m_Adapter->GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);

			if (m_QueueFamilyIndices.Compute != m_QueueFamilyIndices.Graphics)
			{
				auto& createInfo = queueCreateInfos.emplace_back();
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
			m_QueueFamilyIndices.Transfer = m_Adapter->GetQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);

			if ((m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Graphics) && (m_QueueFamilyIndices.Transfer != m_QueueFamilyIndices.Compute))
			{
				auto& createInfo = queueCreateInfos.emplace_back();
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

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32>(enabledDeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

		VK_CHECK(vkCreateDevice(m_Adapter->GetPhysicalDevice(), &deviceCreateInfo, nullptr, &m_Device));

		vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.Graphics, 0, &m_GraphicsQueue);

		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.Graphics;
		VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_GraphicsCommandPool));

		VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
		pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		VK_CHECK(vkCreatePipelineCache(m_Device, &pipelineCacheCreateInfo, nullptr, &m_PipelineCache));
	}

	VulkanDevice::~VulkanDevice()
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		vkDestroyCommandPool(m_Device, m_GraphicsCommandPool, nullptr);
		vkDestroyPipelineCache(m_Device, m_PipelineCache, nullptr);
		vkDestroyDevice(m_Device, nullptr);

		s_Instance = nullptr;
	}

	uint32 VulkanDevice::GetMemoryTypeIndex(uint32 typeBits, VkMemoryPropertyFlags propertyFlags) const
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		VkPhysicalDeviceMemoryProperties memoryProperties;
		vkGetPhysicalDeviceMemoryProperties(m_Adapter->GetPhysicalDevice(), &memoryProperties);

		for (uint32 i = 0; i < memoryProperties.memoryTypeCount; i++)
		{
			if ((typeBits & 1) == 1)
			{
				if ((memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
					return i;
			}
			typeBits >>= 1;
		}

		FLUX_VERIFY(false, "Could not find a matching memory type");
		return 0;
	}

	VkCommandBuffer VulkanDevice::GetCommandBuffer(bool begin) const
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		VkCommandBuffer commandBuffer;

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = m_GraphicsCommandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		VK_CHECK(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, &commandBuffer));

		if (begin)
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		}

		return commandBuffer;
	}

	void VulkanDevice::FlushCommandBuffer(VkCommandBuffer commandBuffer) const
	{
		FLUX_CHECK_IS_RENDER_THREAD();
		FLUX_ASSERT(commandBuffer);
		
		VkDevice device = VulkanDevice::Get()->GetDevice();

		VK_CHECK(vkEndCommandBuffer(commandBuffer));

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		VkFence fence;
		VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence));

		VK_CHECK(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, fence));
		VK_CHECK(vkWaitForFences(device, 1, &fence, VK_TRUE, std::numeric_limits<uint64>::max()));

		vkDestroyFence(m_Device, fence, nullptr);
		vkFreeCommandBuffers(device, m_GraphicsCommandPool, 1, &commandBuffer);
	}

}