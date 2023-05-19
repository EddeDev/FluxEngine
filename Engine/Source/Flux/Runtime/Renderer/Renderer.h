#pragma once

#include "CommandBuffer.h"
#include "Framebuffer.h"

namespace Flux {

	using RenderCommand = std::function<void()>;

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void BeginRenderPass(Ref<CommandBuffer> commandBuffer, Ref<Framebuffer> framebuffer);
		static void EndRenderPass(Ref<CommandBuffer> commandBuffer);

#ifndef FLUX_BUILD_SHIPPING
		static void SubmitRenderCommand(const char* functionName, RenderCommand command);
#else
		static void SubmitRenderCommand(RenderCommand command);
#endif
		static void FlushRenderCommands();

		static uint32 GetCurrentFrameIndex();
	private:
		inline static std::queue<RenderCommand> s_RenderCommandQueue;

#ifndef FLUX_BUILD_SHIPPING
		inline static bool s_RenderCommandQueueLocked = false;
#endif
	};

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_SUBMIT_RENDER_COMMAND(...) Renderer::SubmitRenderCommand(__FUNCTION__, __VA_ARGS__);
#else
	#define FLUX_SUBMIT_RENDER_COMMAND(command) Renderer::SubmitRenderCommand(command);
#endif

}