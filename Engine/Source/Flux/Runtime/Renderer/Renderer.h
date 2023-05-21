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
		static void SubmitRenderCommandRelease(const char* functionName, RenderCommand command);
#else
		static void SubmitRenderCommand(RenderCommand command);
		static void SubmitRenderCommandRelease(RenderCommand command);
#endif
		static void FlushRenderCommands();
		static void FlushReleaseQueue(uint32 frameIndex);

		static uint32 GetCurrentFrameIndex();
		static uint32 RT_GetCurrentFrameIndex();
	private:
		inline static constexpr uint32 s_MaxReleaseQueueCount = 3;

		inline static std::queue<RenderCommand> s_RenderCommandQueue;
		inline static std::queue<RenderCommand> s_ReleaseQueue[s_MaxReleaseQueueCount];

#ifndef FLUX_BUILD_SHIPPING
		inline static std::atomic<bool> s_RenderCommandQueueLocked = false;
		inline static std::atomic<bool> s_ReleaseQueueLocked[s_MaxReleaseQueueCount];
#endif
	};

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_SUBMIT_RENDER_COMMAND(...) Renderer::SubmitRenderCommand(__FUNCTION__, __VA_ARGS__);
#else
	#define FLUX_SUBMIT_RENDER_COMMAND(...) Renderer::SubmitRenderCommand(__VA_ARGS__);
#endif

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_SUBMIT_RENDER_COMMAND_RELEASE(...) Renderer::SubmitRenderCommandRelease(__FUNCTION__, __VA_ARGS__);
#else
	#define FLUX_SUBMIT_RENDER_COMMAND_RELEASE(...) Renderer::SubmitRenderCommandRelease(__VA_ARGS__);
#endif


}