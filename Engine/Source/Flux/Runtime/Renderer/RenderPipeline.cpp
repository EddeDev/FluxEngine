#include "FluxPCH.h"
#include "RenderPipeline.h"

#include "Flux/Runtime/Engine/Engine.h"

#include "Renderer.h"

namespace Flux {

	ForwardRenderPipeline::ForwardRenderPipeline()
	{
		m_ViewportWidth = Engine::Get().GetSwapchain()->GetWidth();
		m_ViewportHeight = Engine::Get().GetSwapchain()->GetHeight();

		uint32 framesInFlight = Renderer::GetFramesInFlight();

		CommandBufferCreateInfo commandBufferCreateInfo;
		commandBufferCreateInfo.Count = framesInFlight;
		commandBufferCreateInfo.Transient = true;
		commandBufferCreateInfo.CreateFromSwapchain = false; // Optional?
		commandBufferCreateInfo.DebugLabel = "Forward RP-CommandBuffer";
		m_CommandBuffer = CommandBuffer::Create(commandBufferCreateInfo);

		// Geometry
		{
			FramebufferCreateInfo framebufferCreateInfo;
			framebufferCreateInfo.Attachments = {
				PixelFormat::RGBA,
				PixelFormat::Depth24Stencil8
			};
			framebufferCreateInfo.ClearColor = { 0.1f, 0.5f, 0.1f, 1.0f };
			framebufferCreateInfo.DebugLabel = "Geometry";

			Framebuffer::Create(framebufferCreateInfo);
		}
	}

	void ForwardRenderPipeline::BeginRendering()
	{
	}

	void ForwardRenderPipeline::EndRendering()
	{
	}

	void ForwardRenderPipeline::BeginRendering2D()
	{
	}

	void ForwardRenderPipeline::EndRendering2D()
	{
	}

	void ForwardRenderPipeline::SetViewportSize(uint32 width, uint32 height)
	{
		FLUX_ASSERT(width > 0 && height > 0);

		if (m_ViewportWidth != width || m_ViewportHeight != height)
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;
		}
	}

}