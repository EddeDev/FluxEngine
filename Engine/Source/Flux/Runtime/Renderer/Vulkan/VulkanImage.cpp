#include "FluxPCH.h"
#include "VulkanImage.h"

#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"

namespace Flux {

	namespace Utils {

		VkFormat VulkanPixelFormat(PixelFormat format)
		{
			switch (format)
			{
			case PixelFormat::RGBA:                 return VK_FORMAT_R8G8B8A8_UNORM;
			case PixelFormat::RGBA16F:              return VK_FORMAT_R16G16B16A16_SFLOAT;
			case PixelFormat::RGBA32F:              return VK_FORMAT_R32G32B32A32_SFLOAT;
			case PixelFormat::Depth24Stencil8:      return VK_FORMAT_D24_UNORM_S8_UINT;
			case PixelFormat::Depth32FStencil8UInt: return VK_FORMAT_D32_SFLOAT_S8_UINT;
			case PixelFormat::Depth32F:             return VK_FORMAT_D32_SFLOAT;
			}
			FLUX_VERIFY(false, "Unknown pixel format");
			return VK_FORMAT_UNDEFINED;
		}

		VkImageUsageFlags VulkanImageUsage(const ImageCreateInfo& createInfo)
		{
			VkImageUsageFlags flags = 0;

			// TODO
			flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

			if (createInfo.Usage == ImageUsage::Attachment)
			{
				if (Utils::IsDepthFormat(createInfo.Format))
					flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
				else
					flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}

			// VK_IMAGE_USAGE_TRANSFER_SRC_BIT
			// VK_IMAGE_USAGE_TRANSFER_DST_BIT
			if (createInfo.Usage == ImageUsage::Storage)
				flags |= VK_IMAGE_USAGE_STORAGE_BIT;

			return flags;
		}

	}

	VulkanImage2D::VulkanImage2D(const ImageCreateInfo& createInfo)
		: m_CreateInfo(createInfo)
	{
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
		Release();
	}

	void VulkanImage2D::Invalidate()
	{
		Ref<VulkanImage2D> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_Invalidate();
		});
	}

	void VulkanImage2D::RT_Invalidate()
	{
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

		__debugbreak();
	}

	void VulkanImage2D::Release()
	{
		delete[] m_Storage;

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([]()
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();

			auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();
		});
	}

	void VulkanImage2D::RT_Release()
	{
		VkDevice device = VulkanDevice::Get()->GetDevice();

		auto& allocator = Renderer::GetResourceAllocator<VulkanResourceAllocator>();
	}

	void VulkanImage2D::AttachToFramebuffer(uint32 attachmentIndex)
	{
		Ref<VulkanImage2D> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, attachmentIndex]() mutable
		{
			instance->RT_AttachToFramebuffer(attachmentIndex);
		});
	}

	void VulkanImage2D::RT_AttachToFramebuffer(uint32 attachmentIndex)
	{
		FLUX_VERIFY(false, "Not implemented");
	}

	void VulkanImage2D::AttachToFramebufferLayer(uint32 attachmentIndex, uint32 layer)
	{
		Ref<VulkanImage2D> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, attachmentIndex, layer]() mutable
		{
			instance->RT_AttachToFramebufferLayer(attachmentIndex, layer);
		});
	}

	void VulkanImage2D::RT_AttachToFramebufferLayer(uint32 attachmentIndex, uint32 layer)
	{
		FLUX_VERIFY(false, "Not implemented");
	}

}