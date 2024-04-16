#pragma once

#include "Flux/Runtime/Core/CommandQueue.h"

namespace Flux {

	class Renderer
	{
	public:
		static void Init(uint32 commandQueueCount);
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

#ifndef FLUX_BUILD_SHIPPING
		template<typename TFunc>
		static void SubmitRenderCommand(const char* functionName, TFunc&& func)
		{
			uint32 queueIndex = GetCurrentQueueIndex();

			if (s_RenderCommandQueueLocked[queueIndex])
			{
				FLUX_CRITICAL_CATEGORY("Renderer", "Recursive call from {0} detected!", functionName);
				FLUX_VERIFY(false);
			}

			s_RenderCommandQueue[queueIndex]->Push(std::forward<TFunc>(func));
		}

		template<typename TFunc>
		static void SubmitRenderCommandRelease(const char* functionName, TFunc&& func)
		{
			SubmitRenderCommand(functionName, [func, functionName]()
			{
				if (s_ReleaseQueueLocked)
				{
					FLUX_CRITICAL_CATEGORY("Renderer", "Recursive call from {0} detected!", functionName);
					FLUX_VERIFY(false);
				}

				s_ReleaseCommandQueue->Push(std::forward<TFunc>((TFunc&&)func));
			});
		}
#else
		template<typename TFunc>
		static void SubmitRenderCommand(TFunc&& func)
		{
			uint32 queueIndex = GetCurrentQueueIndex();

			s_RenderCommandQueue[queueIndex]->Push(std::forward<TFunc>(func));
		}

		template<typename TFunc>
		static void SubmitRenderCommandRelease(TFunc&& func)
		{
			SubmitRenderCommand([func]()
			{
				s_ReleaseCommandQueue->Push(std::forward<TFunc>((TFunc&&)func));
			});
		}
#endif
		static void FlushRenderCommands(uint32 queueIndex = 0);
		static void FlushReleaseQueue();

		static uint32 GetCurrentQueueIndex();
		static uint32 GetQueueCount();
	private:
		inline static constexpr uint32 s_MaxRenderCommandQueueCount = 2;

		inline static CommandQueue* s_RenderCommandQueue[s_MaxRenderCommandQueueCount];
		inline static CommandQueue* s_ReleaseCommandQueue = nullptr;

#ifndef FLUX_BUILD_SHIPPING
		inline static std::atomic<bool> s_RenderCommandQueueLocked[s_MaxRenderCommandQueueCount];
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