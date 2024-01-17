#include "FluxPCH.h"
#include "Renderer.h"

#include "Flux/Runtime/Core/Engine.h"

namespace Flux {

	struct RendererData
	{
		uint32 CurrentQueueCount = 0;
		uint32 CurrentQueueIndex = 0;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Init(uint32 commandQueueCount)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_VERIFY(commandQueueCount <= s_MaxRenderCommandQueueCount);

		s_Data = new RendererData();
		s_Data->CurrentQueueCount = commandQueueCount;
	}

	void Renderer::Shutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer::BeginFrame()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		// Flush release queue
		FLUX_SUBMIT_RENDER_COMMAND([]()
		{
			Renderer::FlushReleaseQueue();
		});
	}

	void Renderer::EndFrame()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		s_Data->CurrentQueueIndex = (s_Data->CurrentQueueIndex + 1) % s_Data->CurrentQueueCount;
	}

#ifndef FLUX_BUILD_SHIPPING
	void Renderer::SubmitRenderCommand(const char* functionName, RenderCommand command)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (s_RenderCommandQueueLocked[s_Data->CurrentQueueIndex])
		{
			FLUX_CRITICAL_CATEGORY("Renderer", "Recursive call from {0} detected!", functionName);
			FLUX_VERIFY(false);
		}

		s_RenderCommandQueue[s_Data->CurrentQueueIndex].push(std::move(command));
	}

	void Renderer::SubmitRenderCommandRelease(const char* functionName, RenderCommand command)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (s_RenderCommandQueueLocked[s_Data->CurrentQueueIndex])
		{
			FLUX_CRITICAL_CATEGORY("Renderer", "Recursive call from {0} detected!", functionName);
			FLUX_VERIFY(false);
		}

		s_RenderCommandQueue[s_Data->CurrentQueueIndex].push([functionName, command = std::move(command)]() mutable
		{
			if (s_ReleaseQueueLocked)
			{
				FLUX_CRITICAL_CATEGORY("Renderer", "Recursive call from {0} detected!", functionName);
				FLUX_VERIFY(false);
			}

			s_ReleaseQueue.push(std::move(command));
		});
	}
#else
	void Renderer::SubmitRenderCommand(RenderCommand command)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		s_RenderCommandQueue.push(std::move(command));
	}

	void Renderer::SubmitRenderCommandRelease(RenderCommand command)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		s_RenderCommandQueue[s_Data->CurrentQueueIndex].push([functionName, command = std::move(command)]() mutable
		{
			s_ReleaseQueue.push(std::move(command));
		});
	}
#endif

	void Renderer::FlushRenderCommands(uint32 queueIndex)
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();

#ifndef FLUX_BUILD_SHIPPING
		if (s_RenderCommandQueueLocked[queueIndex])
		{
			FLUX_CRITICAL_CATEGORY("Renderer", "RT_FlushRenderCommands called recursively!");
			FLUX_VERIFY(false);
		}

		s_RenderCommandQueueLocked[queueIndex] = true;
#endif

		auto& queue = s_RenderCommandQueue[queueIndex];
		while (!queue.empty())
		{
			RenderCommand& command = queue.front();
			command();
			queue.pop();
		}

#ifndef FLUX_BUILD_SHIPPING
		s_RenderCommandQueueLocked[queueIndex] = false;
#endif
	}

	void Renderer::FlushReleaseQueue()
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();

#ifndef FLUX_BUILD_SHIPPING
		if (s_ReleaseQueueLocked)
		{
			FLUX_CRITICAL_CATEGORY("Renderer", "RT_FlushReleaseQueue called recursively!");
			FLUX_VERIFY(false);
		}

		s_ReleaseQueueLocked = true;
#endif

		while (!s_ReleaseQueue.empty())
		{
			RenderCommand& command = s_ReleaseQueue.front();
			command();
			s_ReleaseQueue.pop();
		}

#ifndef FLUX_BUILD_SHIPPING
		s_ReleaseQueueLocked = false;
#endif
	}

	uint32 Renderer::GetCurrentQueueIndex()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		return s_Data->CurrentQueueIndex;
	}

	uint32 Renderer::GetQueueCount()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		return s_Data->CurrentQueueCount;
	}

}