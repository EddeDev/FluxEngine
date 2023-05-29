#pragma once

#include "Flux/Runtime/Renderer/CommandBuffer.h"
#include "Flux/Runtime/Renderer/Image.h"

namespace Flux {

	struct FramebufferImageInfo
	{
		PixelFormat Format = PixelFormat::None;

		FramebufferImageInfo() = default;
		FramebufferImageInfo(PixelFormat format)
			: Format(format) {}
	};

	struct FramebufferAttachmentInfo
	{
		std::vector<FramebufferImageInfo> Attachments;

		FramebufferAttachmentInfo() = default;
		FramebufferAttachmentInfo(const std::initializer_list<FramebufferImageInfo>& attachments)
			: Attachments(attachments) {}
	};

	struct ExistingImage
	{
		Ref<Image2D> Image = nullptr;
		uint32 Layer = 0;
		bool IsLayered = false;

		ExistingImage() = default;
		ExistingImage(Ref<Image2D> image)
			: Image(image) {}
		ExistingImage(Ref<Image2D> image, uint32 layer)
			: Image(image), Layer(layer), IsLayered(true) {}
	};

	struct FramebufferCreateInfo
	{
		uint32 Width = 0;
		uint32 Height = 0;

		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		float DepthClearValue = 1.0f;
		uint32 StencilClearValue = 0;

		bool SwapchainTarget = false;

		FramebufferAttachmentInfo Attachments;
		std::unordered_map<uint32, ExistingImage> ExistingImages;

		std::string DebugLabel = "Framebuffer";
	};

	class Framebuffer : public ReferenceCounted
	{
	public:
		virtual ~Framebuffer() {}

		virtual void Resize(uint32 width, uint32 height, bool forceRecreate = false) = 0;

		virtual void Bind(Ref<CommandBuffer> commandBuffer) const = 0;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer) const = 0;

		virtual void Unbind(Ref<CommandBuffer> commandBuffer) const = 0;
		virtual void RT_Unbind(Ref<CommandBuffer> commandBuffer) const = 0;

		virtual uint32 GetWidth() const = 0;
		virtual uint32 GetHeight() const = 0;

		static Ref<Framebuffer> Create(const FramebufferCreateInfo& createInfo);
	};

}