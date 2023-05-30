#pragma once

#include "Flux/Runtime/Renderer/Image.h"

#include "Vulkan.h"
#include "VulkanResourceAllocator.h"

namespace Flux {

	class VulkanImage2D : public Image2D
	{
	public:
		VulkanImage2D(const ImageCreateInfo& createInfo);
		virtual ~VulkanImage2D();

		virtual void Invalidate() override;
		virtual void RT_Invalidate() override;

		virtual void Release() override;
		virtual void RT_Release() override;

		virtual void AttachToFramebuffer(uint32 attachmentIndex) override;
		virtual void AttachToFramebufferLayer(uint32 attachmentIndex, uint32 layer) override;

		virtual void RT_AttachToFramebuffer(uint32 attachmentIndex) override;
		virtual void RT_AttachToFramebufferLayer(uint32 attachmentIndex, uint32 layer) override;

		virtual ImageCreateInfo& GetCreateInfo() override { return m_CreateInfo; }
		virtual const ImageCreateInfo& GetCreateInfo() const override { return m_CreateInfo; }

		VkImageView GetAttachmentView() const { return m_AttachmentView; }
	private:
		ImageCreateInfo m_CreateInfo;
		uint8* m_Storage = nullptr;

		VkImage m_Image = VK_NULL_HANDLE;
		VkImageView m_AttachmentView = VK_NULL_HANDLE;
		ResourceAllocation m_Allocation = nullptr;
	};

	namespace Utils {

		inline static VkFormat VulkanPixelFormat(PixelFormat format)
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

	}

}