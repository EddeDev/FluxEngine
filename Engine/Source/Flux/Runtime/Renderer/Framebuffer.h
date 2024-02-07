#pragma once

#include "Texture.h"

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
		float DepthClearValue = 0.0f;
		bool ClearColorBuffer = true;
		bool ClearDepthBuffer = true;

		std::vector<FramebufferAttachment> Attachments;

		bool SwapchainTarget = false;
	};

	class Framebuffer : public ReferenceCounted
	{
	public:
		virtual ~Framebuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual Ref<Texture2D> GetColorAttachment(uint32 index = 0) const = 0;
		virtual Ref<Texture2D> GetDepthAttachment() const = 0;

		static Ref<Framebuffer> Create(const FramebufferCreateInfo& createInfo);
	};

}