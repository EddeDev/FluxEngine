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

		virtual Ref<GraphicsContext> GetContext() const override { return m_Context; }

		uint32 GetQueueFamilyIndex(VkQueueFlags queueFlags) const;

		bool IsExtensionSupported(const char* extension) const
		{
			return std::find(m_SupportedExtensions.begin(), m_SupportedExtensions.end(), extension) != m_SupportedExtensions.end();
		}

		VkPhysicalDevice GetPhysicalDevice() const { return m_SelectedDevice; }
	private:
		Ref<VulkanContext> m_Context;

		VkPhysicalDevice m_SelectedDevice = VK_NULL_HANDLE;
		std::vector<std::string> m_SupportedExtensions;

		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
	};

	class VulkanDevice : public GraphicsDevice
	{
	public:
		VulkanDevice(Ref<GraphicsAdapter> adapter);
		virtual ~VulkanDevice();

		virtual Ref<GraphicsAdapter> GetAdapter() const override { return m_Adapter; }

		VkDevice GetDevice() const { return m_Device; }
		VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }

		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

		static Ref<VulkanDevice> Get() { return s_Instance; }
	private:
		inline static VulkanDevice* s_Instance = nullptr;

		Ref<VulkanAdapter> m_Adapter;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue;
		QueueFamilyIndices m_QueueFamilyIndices;
	};


}