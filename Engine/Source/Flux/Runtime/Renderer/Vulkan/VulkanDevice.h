#pragma once

#include "Flux/Runtime/Renderer/GraphicsDevice.h"

#include "VulkanContext.h"

namespace Flux {

	struct QueueFamilyIndices
	{
		uint32 Graphics;
		uint32 Compute;
		uint32 Transfer;
	};

	class VulkanAdapter : public GraphicsAdapter
	{
	public:
		VulkanAdapter(Ref<GraphicsContext> context);
		virtual ~VulkanAdapter() {}

		uint32 GetQueueFamilyIndex(VkQueueFlags queueFlags) const;

		bool IsExtensionSupported(const char* extension) const
		{
			return std::find(m_SupportedExtensions.begin(), m_SupportedExtensions.end(), extension) != m_SupportedExtensions.end();
		}

		VkPhysicalDevice GetPhysicalDevice() const { return m_SelectedDevice; }
	
		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		const std::vector<VkDeviceQueueCreateInfo>& GetQueueCreateInfos() const { return m_QueueCreateInfos; }
	private:
		Ref<VulkanContext> m_Context;

		VkPhysicalDevice m_SelectedDevice = VK_NULL_HANDLE;
		std::vector<std::string> m_SupportedExtensions;

		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;
		QueueFamilyIndices m_QueueFamilyIndices;
	};

	class VulkanDevice : public GraphicsDevice
	{
	public:
		VulkanDevice(Ref<GraphicsAdapter> adapter);
		virtual ~VulkanDevice();
	private:
		Ref<VulkanAdapter> m_Adapter;

		VkDevice m_Device = VK_NULL_HANDLE;
	};


}