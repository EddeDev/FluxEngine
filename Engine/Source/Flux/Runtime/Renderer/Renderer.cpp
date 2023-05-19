#include "FluxPCH.h"
#include "Renderer.h"

#include "Flux/Runtime/Engine/Engine.h"

namespace Flux {

	struct RendererData
	{
		Ref<Framebuffer> ActiveFramebuffer;

		uint32 FrameCount = 0;
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
		s_Data->FrameCount++;
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

#define FLUX_RENDER_COMMAND_QUEUE_DEBUG 0

#ifndef FLUX_BUILD_SHIPPING
	void Renderer::SubmitRenderCommand(const char* functionName, RenderCommand command)
	{
		FLUX_VERIFY_ON_MAIN_THREAD();

#if FLUX_RENDER_COMMAND_QUEUE_DEBUG
		FLUX_TRACE_CATEGORY("Renderer", "Submitting render command from {0}...", functionName);
#endif

		if (s_RenderCommandQueueLocked)
		{
			FLUX_CRITICAL_CATEGORY("Renderer", "Recursive call from {0} detected!", functionName);
			FLUX_VERIFY(false);

			// Should we return here or delaying the recursive call to the next frame?
			return;
		}

		s_RenderCommandQueue.push(std::move(command));
	}
#else
	void Renderer::SubmitRenderCommand(RenderCommand command)
	{
		FLUX_VERIFY_ON_MAIN_THREAD();

#if FLUX_RENDER_COMMAND_QUEUE_DEBUG
		FLUX_TRACE_CATEGORY("Renderer", "Submitting render command...");
#endif

		s_RenderCommandQueue.push(std::move(command));
	}
#endif

	void Renderer::FlushRenderCommands()
	{
		FLUX_ASSERT_ON_MAIN_THREAD();
		
#ifndef FLUX_BUILD_SHIPPING
		s_RenderCommandQueueLocked = true;
#endif

#if FLUX_RENDER_COMMAND_QUEUE_DEBUG
		FLUX_WARNING_CATEGORY("Renderer", "Executing {0} render commands...", s_RenderCommandQueue.size());
#endif

		while (!s_RenderCommandQueue.empty())
		{
			RenderCommand& command = s_RenderCommandQueue.front();
			command();
			s_RenderCommandQueue.pop();
		}

#ifndef FLUX_BUILD_SHIPPING
		s_RenderCommandQueueLocked = false;
#endif
	}

	uint32 Renderer::GetCurrentFrameIndex()
	{
		return Engine::Get().GetSwapchain()->GetCurrentBufferIndex();
	}

}