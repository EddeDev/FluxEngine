#include "FluxPCH.h"
#include "VulkanSwapchain.h"

#include "VulkanDevice.h"

namespace Flux {

#ifdef FLUX_PLATFORM_WINDOWS
	extern HINSTANCE g_Instance;
#endif

	VulkanSwapchain::VulkanSwapchain(Window* window)
		: m_Window(window)
	{
		Ref<VulkanAdapter> adapter = VulkanDevice::Get()->GetAdapter().As<VulkanAdapter>();
		Ref<VulkanContext> context = adapter->GetContext().As<VulkanContext>();

		m_Instance = context->GetInstance();
		m_PhysicalDevice = adapter->GetPhysicalDevice();
		m_Device = VulkanDevice::Get()->GetDevice();

		CreateSurface();
		FindPresentQueue();
		FindColorFormat();

		CreateSwapchain();
		CreateImageViews();
		CreateRenderPass();
		CreateDepthStencilImage();
		CreateFramebuffer();
		CreateSemaphores();
		CreateCommandBuffers();
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		DestroyCommandBuffers();
		DestroySemaphores();
		DestroyFramebuffer();
		DestroyDepthStencilImage();
		DestroyRenderPass();
		DestroyImageViews();
		DestroySwapchain();
		DestroySurface();
	}

	void VulkanSwapchain::BeginFrame()
	{
		VkResult result = vkAcquireNextImageKHR(m_Device, m_Swapchain, std::numeric_limits<uint64>::max(), m_PresentComplete, VK_NULL_HANDLE, &m_CurrentBufferIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			VK_CHECK(vkDeviceWaitIdle(m_Device));

			VkSurfaceCapabilitiesKHR surfaceCapabilities;
			VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities));

			VkExtent2D extent = ChooseExtent(surfaceCapabilities);
			if (extent.width > 0 && extent.height > 0)
			{
				DestroyCommandBuffers();
				DestroySemaphores();
				DestroyFramebuffer();
				DestroyDepthStencilImage();
				DestroyRenderPass();
				DestroyImageViews();
				DestroySwapchain();

				CreateSwapchain();
				CreateImageViews();
				CreateRenderPass();
				CreateDepthStencilImage();
				CreateFramebuffer();
				CreateSemaphores();
				CreateCommandBuffers();
			}

			VK_CHECK(vkDeviceWaitIdle(m_Device));
		}

		VK_CHECK(vkResetCommandPool(m_Device, m_CommandPools[m_CurrentBufferIndex], 0));
	}

	void VulkanSwapchain::Present(int32 swapInterval)
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities));

		VkExtent2D extent = ChooseExtent(surfaceCapabilities);
		if (extent.width == 0 || extent.height == 0)
			return;

		VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = &submitPipelineStages;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_PresentComplete;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_RenderComplete;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentBufferIndex];

		VK_CHECK(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, nullptr));

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pImageIndices = &m_CurrentBufferIndex;

		if (m_RenderComplete)
		{
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &m_RenderComplete;
		}

		VkResult result = vkQueuePresentKHR(m_GraphicsQueue, &presentInfo);

		if (m_SwapInterval != swapInterval)
		{
			m_SwapInterval = swapInterval;
			result = VK_ERROR_OUT_OF_DATE_KHR;
		}

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			VK_CHECK(vkDeviceWaitIdle(m_Device));

			VkSurfaceCapabilitiesKHR surfaceCapabilities;
			VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities));

			VkExtent2D extent = ChooseExtent(surfaceCapabilities);
			if (extent.width > 0 && extent.height > 0)
			{
				DestroyCommandBuffers();
				DestroySemaphores();
				DestroyFramebuffer();
				DestroyDepthStencilImage();
				DestroyRenderPass();
				DestroyImageViews();
				DestroySwapchain();

				CreateSwapchain();
				CreateImageViews();
				CreateRenderPass();
				CreateDepthStencilImage();
				CreateFramebuffer();
				CreateSemaphores();
				CreateCommandBuffers();
			}

			VK_CHECK(vkDeviceWaitIdle(m_Device));
		}

		VK_CHECK(vkQueueWaitIdle(m_GraphicsQueue));
	}

	void VulkanSwapchain::CreateSurface()
	{
#ifdef FLUX_PLATFORM_WINDOWS
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
		surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceCreateInfo.hwnd = (HWND)m_Window->GetNativeHandle();
		surfaceCreateInfo.hinstance = g_Instance;
		VK_CHECK(vkCreateWin32SurfaceKHR(m_Instance, &surfaceCreateInfo, nullptr, &m_Surface));
#endif
	}

	void VulkanSwapchain::CreateSwapchain()
	{
		uint32 surfaceFormatCount;
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatCount, nullptr));
		std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatCount, surfaceFormats.data()));

		uint32 presentModeCount;
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, nullptr));
		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, presentModes.data()));

		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities));

		VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(surfaceFormats);
		VkPresentModeKHR presentMode = ChoosePresentMode(presentModes);
		VkCompositeAlphaFlagBitsKHR compositeAlpha = ChooseCompositeAlpha(surfaceCapabilities);
		VkExtent2D extent = ChooseExtent(surfaceCapabilities);

		uint32 imageCount = surfaceCapabilities.minImageCount + 1;
		if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
			imageCount = surfaceCapabilities.maxImageCount;

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = m_Surface;
		swapchainCreateInfo.minImageCount = imageCount;
		swapchainCreateInfo.imageFormat = m_ColorFormat;
		swapchainCreateInfo.imageColorSpace = m_ColorSpace;
		swapchainCreateInfo.imageExtent = extent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = compositeAlpha;
		swapchainCreateInfo.presentMode = presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;

		if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		VK_CHECK(vkCreateSwapchainKHR(m_Device, &swapchainCreateInfo, nullptr, &m_Swapchain));

		m_Width = extent.width;
		m_Height = extent.height;
	}

	void VulkanSwapchain::CreateImageViews()
	{
		VK_CHECK(vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &m_ImageCount, nullptr));
		FLUX_VERIFY(m_ImageCount > 0);

		std::vector<VkImage> images(m_ImageCount);
		VK_CHECK(vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &m_ImageCount, images.data()));

		m_SwapchainImageViews.resize(m_ImageCount);

		for (uint32 i = 0; i < m_ImageCount; i++)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.format = m_ColorFormat;
			imageViewCreateInfo.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;
			imageViewCreateInfo.image = images[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

			VK_CHECK(vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr, &m_SwapchainImageViews[i]));
		}
	}

	void VulkanSwapchain::CreateRenderPass()
	{
		std::array<VkAttachmentDescription, 2> attachmentDescriptions;

		// Color attachment
		attachmentDescriptions[0].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
		attachmentDescriptions[0].format = m_ColorFormat;
		attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// Depth attachment
		attachmentDescriptions[1].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
		attachmentDescriptions[1].format = m_DepthFormat;
		attachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentReference = {};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentReference = {};
		depthAttachmentReference.attachment = 1;
		depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentReference;
		subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

		std::array<VkSubpassDependency, 2> subpassDependencies;

		subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependencies[0].dstSubpass = 0;
		subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		subpassDependencies[1].srcSubpass = 0;
		subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; 
		subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = static_cast<uint32>(attachmentDescriptions.size());
		renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = static_cast<uint32>(subpassDependencies.size());
		renderPassCreateInfo.pDependencies = subpassDependencies.data();

		VK_CHECK(vkCreateRenderPass(m_Device, &renderPassCreateInfo, nullptr, &m_RenderPass));
	}

	void VulkanSwapchain::CreateDepthStencilImage()
	{
		Ref<VulkanDevice> device = VulkanDevice::Get();

		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = m_DepthFormat;
		imageCreateInfo.extent = { m_Width, m_Height, 1 };
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

		VK_CHECK(vkCreateImage(m_Device, &imageCreateInfo, nullptr, &m_DepthStencilImage));

		VkMemoryRequirements memoryRequirments;
		vkGetImageMemoryRequirements(m_Device, m_DepthStencilImage, &memoryRequirments);

		VkMemoryAllocateInfo memoryAllocateInfo = {};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memoryRequirments.size;
		memoryAllocateInfo.memoryTypeIndex = device->GetMemoryTypeIndex(memoryRequirments.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK(vkAllocateMemory(m_Device, &memoryAllocateInfo, nullptr, &m_DepthStencilImageMemory));
		VK_CHECK(vkBindImageMemory(m_Device, m_DepthStencilImage, m_DepthStencilImageMemory, 0));

		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.image = m_DepthStencilImage;
		imageViewCreateInfo.format = m_DepthFormat;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (m_DepthFormat >= VK_FORMAT_D16_UNORM_S8_UINT)
			imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		VK_CHECK(vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr, &m_DepthStencilImageView));
	}

	void VulkanSwapchain::CreateFramebuffer()
	{
		m_Framebuffers.resize(m_ImageCount);
		for (uint32 i = 0; i < m_ImageCount; i++)
		{
			std::array<VkImageView, 2> attachments;
			attachments[0] = m_SwapchainImageViews[i];
			attachments[1] = m_DepthStencilImageView;

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = m_RenderPass;
			framebufferCreateInfo.attachmentCount = static_cast<uint32>(attachments.size());
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = m_Width;
			framebufferCreateInfo.height = m_Height;
			framebufferCreateInfo.layers = 1;

			VK_CHECK(vkCreateFramebuffer(m_Device, &framebufferCreateInfo, nullptr, &m_Framebuffers[i]));
		}
	}

	void VulkanSwapchain::CreateSemaphores()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_PresentComplete));
		VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_RenderComplete));
	}

	void VulkanSwapchain::CreateCommandBuffers()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = m_PresentQueueIndex;

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		m_CommandPools.resize(m_ImageCount);
		m_CommandBuffers.resize(m_ImageCount);
		for (uint32 i = 0; i < m_ImageCount; i++)
		{
			VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_CommandPools[i]));
			commandBufferAllocateInfo.commandPool = m_CommandPools[i];
			VK_CHECK(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, &m_CommandBuffers[i]));
		}
	}

	void VulkanSwapchain::DestroySurface()
	{
		if (m_Surface)
		{
			vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
			m_Surface = VK_NULL_HANDLE;
		}
	}

	void VulkanSwapchain::DestroySwapchain()
	{
		if (m_Swapchain)
		{
			vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
			m_Swapchain = VK_NULL_HANDLE;
		}
	}

	void VulkanSwapchain::DestroyImageViews()
	{
		for (uint32 i = 0; i < static_cast<uint32>(m_SwapchainImageViews.size()); i++)
			vkDestroyImageView(m_Device, m_SwapchainImageViews[i], nullptr);
		m_SwapchainImageViews.clear();
	}

	void VulkanSwapchain::DestroyRenderPass()
	{
		if (m_RenderPass)
		{
			vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
			m_RenderPass = VK_NULL_HANDLE;
		}
	}

	void VulkanSwapchain::DestroyDepthStencilImage()
	{
		if (m_DepthStencilImageView)
		{
			vkDestroyImageView(m_Device, m_DepthStencilImageView, nullptr);
			m_DepthStencilImageView = VK_NULL_HANDLE;
		}
	
		if (m_DepthStencilImageMemory)
		{
			vkFreeMemory(m_Device, m_DepthStencilImageMemory, nullptr);
			m_DepthStencilImageMemory = VK_NULL_HANDLE;
		}

		if (m_DepthStencilImage)
		{
			vkDestroyImage(m_Device, m_DepthStencilImage, nullptr);
			m_DepthStencilImage = VK_NULL_HANDLE;
		}
	}

	void VulkanSwapchain::DestroyFramebuffer()
	{
		for (uint32 i = 0; i < static_cast<uint32>(m_Framebuffers.size()); i++)
			vkDestroyFramebuffer(m_Device, m_Framebuffers[i], nullptr);
		m_Framebuffers.clear();
	}

	void VulkanSwapchain::DestroySemaphores()
	{
		if (m_PresentComplete)
		{
			vkDestroySemaphore(m_Device, m_PresentComplete, nullptr);
			m_PresentComplete = VK_NULL_HANDLE;
		}

		if (m_RenderComplete)
		{
			vkDestroySemaphore(m_Device, m_RenderComplete, nullptr);
			m_RenderComplete = VK_NULL_HANDLE;
		}
	}

	void VulkanSwapchain::DestroyCommandBuffers()
	{
		for (uint32 i = 0; i < static_cast<uint32>(m_CommandBuffers.size()); i++)
			vkFreeCommandBuffers(m_Device, m_CommandPools[i], 1, &m_CommandBuffers[i]);
		m_CommandBuffers.clear();
		
		for (uint32 i = 0; i < static_cast<uint32>(m_CommandPools.size()); i++)
			vkDestroyCommandPool(m_Device, m_CommandPools[i], nullptr);
		m_CommandPools.clear();
	}

	void VulkanSwapchain::FindPresentQueue()
	{
		uint32 queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
		FLUX_VERIFY(queueFamilyCount > 0);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		std::vector<VkBool32> supportsPresent(queueFamilyCount);
		for (uint32 i = 0; i < queueFamilyCount; i++)
			VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &supportsPresent[i]));

		uint32 graphicsQueueIndex = std::numeric_limits<uint32>::max();
		uint32 presentQueueIndex = std::numeric_limits<uint32>::max();

		for (uint32 i = 0; i < queueFamilyCount; i++)
		{
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (graphicsQueueIndex == std::numeric_limits<uint32>::max())
					graphicsQueueIndex = i;

				if (supportsPresent[i] == VK_TRUE)
				{
					graphicsQueueIndex = i;
					presentQueueIndex = i;
					break;
				}
			}
		}

		if (presentQueueIndex == std::numeric_limits<uint32>::max())
		{
			for (uint32 i = 0; i < queueFamilyCount; i++)
			{
				if (supportsPresent[i] == VK_TRUE)
				{
					presentQueueIndex = i;
					break;
				}
			}
		}

		FLUX_VERIFY(graphicsQueueIndex != std::numeric_limits<uint32>::max(), "Could not find graphics queue.");
		FLUX_VERIFY(presentQueueIndex != std::numeric_limits<uint32>::max(), "Could not find presenting queue.");
		FLUX_VERIFY(graphicsQueueIndex == presentQueueIndex, "Separate graphics and presenting queues are not supported.");

		m_PresentQueueIndex = presentQueueIndex;

		vkGetDeviceQueue(m_Device, m_PresentQueueIndex, 0, &m_GraphicsQueue);
	}
	
	void VulkanSwapchain::FindColorFormat()
	{
		uint32 surfaceFormatCount;
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatCount, nullptr));
		FLUX_VERIFY(surfaceFormatCount > 0);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatCount, surfaceFormats.data()));
		
		VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(surfaceFormats);
		m_ColorFormat = surfaceFormat.format;
		m_ColorSpace = surfaceFormat.colorSpace;
		m_DepthFormat = ChooseDepthFormat();
	}

	VkFormat VulkanSwapchain::ChooseDepthFormat() const
	{
		const std::vector<VkFormat> depthFormats = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};

		VkFormatProperties formatProperties;
		for (auto format : depthFormats)
		{
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &formatProperties);
			if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT)
				return format;
		}

		return VK_FORMAT_UNDEFINED;
	}

	VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) const
	{
		const VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;

		if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
			return { colorFormat, surfaceFormats[0].colorSpace };

		for (const auto& surfaceFormat : surfaceFormats)
		{
			if (surfaceFormat.format == colorFormat)
				return surfaceFormat;
		}

		return surfaceFormats[0];
	}

	VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) const
	{
		if (m_SwapInterval == 0)
		{
			for (const auto& presentMode : presentModes)
			{
				if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					return presentMode;
				if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
					return presentMode;
			}
		}

		if (m_SwapInterval == 1)
			return VK_PRESENT_MODE_FIFO_KHR;
		if (m_SwapInterval == -1)
			return VK_PRESENT_MODE_FIFO_RELAXED_KHR;

		FLUX_VERIFY(false, "Swap interval must be in range: [-1, 1]");
		return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}

	VkCompositeAlphaFlagBitsKHR VulkanSwapchain::ChooseCompositeAlpha(const VkSurfaceCapabilitiesKHR& capabilities) const
	{
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR
		};

		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		for (auto compositeAlphaFlag : compositeAlphaFlags)
		{
			if (capabilities.supportedCompositeAlpha & compositeAlphaFlag)
			{
				compositeAlpha = compositeAlphaFlag;
				break;
			}
		}

		return compositeAlpha;
	}

	VkExtent2D VulkanSwapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32>::max())
			return capabilities.currentExtent;
		
		VkExtent2D extent = { m_Width, m_Height };
		extent.width = glm::clamp(extent.width, capabilities.minImageExtent.width, capabilities.minImageExtent.width);
		extent.height = glm::clamp(extent.height, capabilities.minImageExtent.height, capabilities.minImageExtent.height);
		return extent;
	}

}