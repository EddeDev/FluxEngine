#include "FluxPCH.h"
#include "VulkanImage.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"

namespace Flux {

	namespace Utils {

		static VkImageUsageFlags VulkanImageUsage(const ImageCreateInfo& createInfo)
		{
			VkImageUsageFlags flags = VK_IMAGE_USAGE_SAMPLED_BIT;

			if (createInfo.Usage == ImageUsage::Attachment)
			{
				if (Utils::IsDepthFormat(createInfo.Format))
					flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				else
					flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}

			if (createInfo.Usage == ImageUsage::Storage)
				flags |= VK_IMAGE_USAGE_STORAGE_BIT;

			return flags;
		}

	}

	VulkanImage2D::VulkanImage2D(const ImageCreateInfo& createInfo)
		: m_CreateInfo(createInfo)
	{
		FLUX_ASSERT_IS_MAIN_THREAD();
		FLUX_ASSERT(createInfo.Width > 0 && createInfo.Height > 0);

		if (createInfo.InitialData)
		{
			uint32 dataSize = createInfo.Width * createInfo.Height * createInfo.ArrayLayers * Utils::ComputeBytesPerPixel(createInfo.Format);
			m_Storage = new uint8[dataSize];
			memcpy(m_Storage, createInfo.InitialData, dataSize);
		}
	}

	VulkanImage2D::~VulkanImage2D()
	{
		FLUX_ASSERT_IS_MAIN_THREAD();

		Release();
	}

	void VulkanImage2D::Invalidate()
	{
		FLUX_ASSERT_IS_MAIN_THREAD();

		Ref<VulkanImage2D> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_Invalidate();
		});
	}

	void VulkanImage2D::RT_Invalidate()
	{
		FLUX_ASSERT_IS_RENDER_THREAD();

		RT_Release();

		VkDevice device = VulkanDevice::Get()->GetDevice();

		auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();
		
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = Utils::VulkanPixelFormat(m_CreateInfo.Format);
		imageCreateInfo.extent.width = m_CreateInfo.Width;
		imageCreateInfo.extent.height = m_CreateInfo.Height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = m_CreateInfo.MipLevels;
		imageCreateInfo.arrayLayers = m_CreateInfo.ArrayLayers;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = Utils::VulkanImageUsage(m_CreateInfo);

		m_Allocation = allocator.CreateImage(imageCreateInfo, ResourceMemoryUsage::GpuOnly, m_Image);
	}

	void VulkanImage2D::Release()
	{
		FLUX_ASSERT_IS_MAIN_THREAD();

		delete[] m_Storage;

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([image = m_Image, allocation = m_Allocation]()
		{
			if (image)
			{
				auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();
				allocator.DestroyImage(image, allocation);
			}
		});
	}

	void VulkanImage2D::RT_Release()
	{
		FLUX_ASSERT_IS_RENDER_THREAD();

		if (m_Image)
		{
			auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();
			allocator.DestroyImage(m_Image, m_Allocation);
			m_Image = VK_NULL_HANDLE;
		}
	}

	void VulkanImage2D::AttachToFramebuffer(uint32 attachmentIndex)
	{
		FLUX_ASSERT_IS_MAIN_THREAD();

		Ref<VulkanImage2D> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, attachmentIndex]() mutable
		{
			instance->RT_AttachToFramebuffer(attachmentIndex);
		});
	}

	void VulkanImage2D::RT_AttachToFramebuffer(uint32 attachmentIndex)
	{
		FLUX_ASSERT_IS_RENDER_THREAD();
		FLUX_VERIFY(m_CreateInfo.Usage == ImageUsage::Attachment);

		VkDevice device = VulkanDevice::Get()->GetDevice();

		VkImageAspectFlags aspectMask = 0;
		if (Utils::IsDepthFormat(m_CreateInfo.Format))
		{
			aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;

			if (Utils::IsStencilFormat(m_CreateInfo.Format))
				aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else
		{
			aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
		}

		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = aspectMask;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = m_CreateInfo.MipLevels;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = m_CreateInfo.ArrayLayers;

		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = m_Image;
		imageViewCreateInfo.viewType = m_CreateInfo.ArrayLayers > 0 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = Utils::VulkanPixelFormat(m_CreateInfo.Format);
		imageViewCreateInfo.subresourceRange = subresourceRange;

		if (m_AttachmentView)
			vkDestroyImageView(device, m_AttachmentView, nullptr);

		VK_CHECK(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_AttachmentView));
	}

	void VulkanImage2D::AttachToFramebufferLayer(uint32 attachmentIndex, uint32 layer)
	{
		FLUX_ASSERT_IS_MAIN_THREAD();

		Ref<VulkanImage2D> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, attachmentIndex, layer]() mutable
		{
			instance->RT_AttachToFramebufferLayer(attachmentIndex, layer);
		});
	}

	void VulkanImage2D::RT_AttachToFramebufferLayer(uint32 attachmentIndex, uint32 layer)
	{
		FLUX_ASSERT_IS_RENDER_THREAD();
		FLUX_VERIFY(m_CreateInfo.Usage == ImageUsage::Attachment);

		VkDevice device = VulkanDevice::Get()->GetDevice();

		VkImageAspectFlags aspectMask = 0;
		if (Utils::IsDepthFormat(m_CreateInfo.Format))
		{
			aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;

			if (Utils::IsStencilFormat(m_CreateInfo.Format))
				aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else
		{
			aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
		}

		VkImageSubresourceRange subresourceRange = {};
		subresourceRange.aspectMask = aspectMask;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = m_CreateInfo.MipLevels;
		subresourceRange.baseArrayLayer = layer;
		subresourceRange.layerCount = m_CreateInfo.ArrayLayers;

		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = m_Image;
		imageViewCreateInfo.viewType = m_CreateInfo.ArrayLayers > 0 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = Utils::VulkanPixelFormat(m_CreateInfo.Format);
		imageViewCreateInfo.subresourceRange = subresourceRange;

		if (m_AttachmentView)
			vkDestroyImageView(device, m_AttachmentView, nullptr);

		VK_CHECK(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_AttachmentView));
	}

}