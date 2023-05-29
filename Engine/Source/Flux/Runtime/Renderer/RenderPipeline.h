#pragma once

#include "CommandBuffer.h"
#include "GraphicsPipeline.h"

namespace Flux {

	class RenderPipeline : public ReferenceCounted
	{
	public:
		virtual ~RenderPipeline() {}

		virtual void BeginRendering() = 0;
		virtual void EndRendering() = 0;

		virtual void BeginRendering2D() = 0;
		virtual void EndRendering2D() = 0;

		virtual void SetViewportSize(uint32 width, uint32 height) = 0;
	};

	class ForwardRenderPipeline : public RenderPipeline
	{
	public:
		ForwardRenderPipeline();

		virtual void BeginRendering() override;
		virtual void EndRendering() override;

		virtual void BeginRendering2D() override;
		virtual void EndRendering2D() override;

		virtual void SetViewportSize(uint32 width, uint32 height) override;
	private:
		uint32 m_ViewportWidth = 0;
		uint32 m_ViewportHeight = 0;

		Ref<CommandBuffer> m_CommandBuffer;
	};

}