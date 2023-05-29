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
	private:
		ImageCreateInfo m_CreateInfo;
		uint8* m_Storage = nullptr;

		VkImage m_Image = VK_NULL_HANDLE;
		ResourceAllocation m_Allocation = nullptr;
	};

}