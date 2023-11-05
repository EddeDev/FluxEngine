#pragma once

namespace Flux {

	using RenderCommand = std::function<void()>;

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

#ifndef FLUX_BUILD_SHIPPING
		static void SubmitRenderCommand(const char* functionName, RenderCommand command);
		static void SubmitRenderCommandRelease(const char* functionName, RenderCommand command);
#else
		static void SubmitRenderCommand(RenderCommand command);
		static void SubmitRenderCommandRelease(RenderCommand command);
#endif
		static void FlushRenderCommands(int32 queueIndex);
		static void FlushReleaseQueue();

		static uint32 GetCurrentQueueIndex();
	private:
		inline static constexpr uint32 s_RenderCommandQueueCount = 2;

		inline static std::queue<RenderCommand> s_RenderCommandQueue[s_RenderCommandQueueCount];
		inline static std::queue<RenderCommand> s_ReleaseQueue;

#ifndef FLUX_BUILD_SHIPPING
		inline static std::atomic<bool> s_RenderCommandQueueLocked[s_RenderCommandQueueCount];
		inline static std::atomic<bool> s_ReleaseQueueLocked;
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