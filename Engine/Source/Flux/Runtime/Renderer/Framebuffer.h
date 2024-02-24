#pragma once

#include "Texture.h"
#include "CompareFunction.h"

namespace Flux {

	struct FramebufferAttachment
	{
		TextureFormat Format;

		FramebufferAttachment() = default;
		FramebufferAttachment(TextureFormat format)
			: Format(format) {}
	};

	struct FramebufferCreateInfo
	{
		uint32 Width = 0;
		uint32 Height = 0;

		Vector4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		bool ClearColorBuffer = true;

		float DepthClearValue = 0.0f;
		CompareFunction DepthCompareFunction = CompareFunction::GreaterOrEqual;
		bool ClearDepthBuffer = true;

		bool SwapchainTarget = false;

		std::vector<FramebufferAttachment> Attachments;

		std::string DebugLabel;
	};

	class Framebuffer : public ReferenceCounted
	{
	public:
		virtual ~Framebuffer() {}

		virtual void Invalidate() = 0;
		virtual void Resize(uint32 width, uint32 height) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual Ref<Texture> GetColorAttachment(uint32 index = 0) const = 0;
		virtual Ref<Texture> GetDepthAttachment() const = 0;

		static Ref<Framebuffer> Create(const FramebufferCreateInfo& createInfo);
	};

}