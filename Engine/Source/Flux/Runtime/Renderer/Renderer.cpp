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
	
		for (uint32 i = 0; i < commandQueueCount; i++)
			s_RenderCommandQueue[i] = new CommandQueue(fmt::format("Renderer - Render Command Queue [{0}]", i), 1024 * 1024);

		s_ReleaseCommandQueue = new CommandQueue("Renderer - Release Command Queue", 1024);
	}

	void Renderer::Shutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		for (uint32 i = 0; i < s_MaxRenderCommandQueueCount; i++)
		{
			delete s_RenderCommandQueue[i];
			s_RenderCommandQueue[i] = nullptr;
		}

		delete s_ReleaseCommandQueue;
		s_ReleaseCommandQueue = nullptr;

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

		s_RenderCommandQueue[queueIndex]->Flush();

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

		s_ReleaseCommandQueue->Flush();

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