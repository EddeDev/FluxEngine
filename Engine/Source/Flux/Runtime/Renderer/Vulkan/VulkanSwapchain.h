#pragma once

#include "Flux/Runtime/Renderer/Swapchain.h"

#include "Vulkan.h"

namespace Flux {

	class VulkanSwapchain : public Swapchain
	{
	public:
		VulkanSwapchain(Window* window);
		virtual ~VulkanSwapchain();

		virtual void BeginFrame() override;
		virtual void Present(int32 swapInterval) override;

		virtual uint32 GetWidth() const override { return m_Width; }
		virtual uint32 GetHeight() const override { return m_Height; }

		VkRenderPass GetRenderPass() const { return m_RenderPass; }

		VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffers.at(m_CurrentBufferIndex); }
		VkFramebuffer GetFramebuffer() const { return m_Framebuffers.at(m_CurrentBufferIndex); }
	private:
		void CreateSurface();
		void CreateSwapchain();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateDepthStencilImage();
		void CreateFramebuffer();
		void CreateSemaphores();
		void CreateCommandBuffers();

		void DestroySurface();
		void DestroySwapchain();
		void DestroyImageViews();
		void DestroyRenderPass();
		void DestroyDepthStencilImage();
		void DestroyFramebuffer();
		void DestroySemaphores();
		void DestroyCommandBuffers();

		void FindPresentQueue();
		void FindColorFormat();

		VkFormat ChooseDepthFormat() const;
		VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) const;
		VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes) const;
		VkCompositeAlphaFlagBitsKHR ChooseCompositeAlpha(const VkSurfaceCapabilitiesKHR& capabilities) const;
		VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
	private:
		Window* m_Window = nullptr;
		uint32 m_Width = 0, m_Height = 0;
		int32 m_SwapInterval = 1;

		VkInstance m_Instance = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;

		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		VkImage m_DepthStencilImage = VK_NULL_HANDLE;
		VkDeviceMemory m_DepthStencilImageMemory = VK_NULL_HANDLE;
		VkImageView m_DepthStencilImageView = VK_NULL_HANDLE;

		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		uint32 m_PresentQueueIndex = 0;

		VkFormat m_ColorFormat = VK_FORMAT_UNDEFINED;
		VkColorSpaceKHR m_ColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;

		std::vector<VkImageView> m_SwapchainImageViews;
		std::vector<VkFramebuffer> m_Framebuffers;

		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<VkCommandPool> m_CommandPools;

		VkSemaphore m_PresentComplete = VK_NULL_HANDLE;
		VkSemaphore m_RenderComplete = VK_NULL_HANDLE;

		uint32 m_ImageCount = 0;
		uint32 m_CurrentBufferIndex = 0;
	};

}