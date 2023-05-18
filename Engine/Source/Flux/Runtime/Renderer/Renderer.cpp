#include "FluxPCH.h"
#include "Renderer.h"

#include "Flux/Runtime/Engine/Engine.h"

namespace Flux {

	struct RendererData
	{
		Ref<Framebuffer> ActiveFramebuffer;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Init()
	{
		s_Data = new RendererData();
	}

	void Renderer::Shutdown()
	{
		s_Data = nullptr;
	}

	void Renderer::BeginFrame()
	{
	}

	void Renderer::EndFrame()
	{
	}

	void Renderer::BeginRenderPass(Ref<CommandBuffer> commandBuffer, Ref<Framebuffer> framebuffer)
	{
		FLUX_ASSERT(!s_Data->ActiveFramebuffer);
		FLUX_ASSERT(commandBuffer);
		FLUX_ASSERT(framebuffer);

		s_Data->ActiveFramebuffer = framebuffer;
		framebuffer->Bind(commandBuffer);
	}

	void Renderer::EndRenderPass(Ref<CommandBuffer> commandBuffer)
	{
		FLUX_ASSERT(s_Data->ActiveFramebuffer);
		FLUX_ASSERT(commandBuffer);

		s_Data->ActiveFramebuffer->Unbind(commandBuffer);
		s_Data->ActiveFramebuffer = nullptr;
	}

	uint32 Renderer::GetCurrentFrameIndex()
	{
		return Engine::Get().GetSwapchain()->GetCurrentBufferIndex();
	}

}