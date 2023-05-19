#pragma once

#include "Flux/Runtime/Renderer/CommandBuffer.h"

namespace Flux {

	struct FramebufferCreateInfo
	{
		uint32 Width = 0;
		uint32 Height = 0;

		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		float DepthClearValue = 1.0f;
		uint32 StencilClearValue = 0;

		bool SwapchainTarget = false;
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