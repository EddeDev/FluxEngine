#include "FluxPCH.h"
#include "Renderer.h"

#include "Flux/Runtime/Engine/Engine.h"

#include "Vulkan/VulkanResourceAllocator.h"

namespace Flux {

	struct RendererData
	{
		Ref<Framebuffer> ActiveFramebuffer;

		uint32 FrameCount = 0;
	};

	static RendererData* s_Data = nullptr;

	static ResourceAllocator* CreateResourceAllocator()
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
		case GraphicsAPI::Vulkan: return new VulkanResourceAllocator();
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

	void Renderer::Init()
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		s_Data = new RendererData();
		s_ResourceAllocator = CreateResourceAllocator();
	}

	void Renderer::Shutdown()
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		for (uint32 frameIndex = 0; frameIndex < s_MaxReleaseQueueCount; frameIndex++)
			FlushReleaseQueue(frameIndex);

		delete s_ResourceAllocator;
		s_ResourceAllocator = nullptr;
		
		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer::BeginFrame()
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		s_Data->FrameCount++;
	}

	void Renderer::EndFrame()
	{
		FLUX_ASSERT_ON_MAIN_THREAD();
	}

	void Renderer::BeginRenderPass(Ref<CommandBuffer> commandBuffer, Ref<Framebuffer> framebuffer)
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		FLUX_ASSERT(!s_Data->ActiveFramebuffer);
		FLUX_ASSERT(commandBuffer);
		FLUX_ASSERT(framebuffer);

		s_Data->ActiveFramebuffer = framebuffer;
		framebuffer->Bind(commandBuffer);
	}

	void Renderer::EndRenderPass(Ref<CommandBuffer> commandBuffer)
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		FLUX_ASSERT(s_Data->ActiveFramebuffer);
		FLUX_ASSERT(commandBuffer);

		s_Data->ActiveFramebuffer->Unbind(commandBuffer);
		s_Data->ActiveFramebuffer = nullptr;
	}

	void Renderer::RenderGeometry(Ref<CommandBuffer> commandBuffer, Ref<GraphicsPipeline> pipeline, Ref<Shader> shader, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer)
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		vertexBuffer->Bind(commandBuffer);
		pipeline->Bind(commandBuffer);
		indexBuffer->Bind(commandBuffer);

		pipeline->DrawIndexed(commandBuffer, indexBuffer->GetCount());
	}

#ifndef FLUX_BUILD_SHIPPING
	void Renderer::SubmitRenderCommand(const char* functionName, RenderCommand command)
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		if (s_RenderCommandQueueLocked)
		{
			FLUX_CRITICAL_CATEGORY("Renderer", "Recursive call from {0} detected!", functionName);
			FLUX_VERIFY(false);

			// Should we return here or delay the recursive call to the next frame?
			return;
		}

		s_RenderCommandQueue.push(std::move(command));
	}

	void Renderer::SubmitRenderCommandRelease(const char* functionName, RenderCommand command)
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		uint32 frameIndex = Renderer::GetCurrentFrameIndex();

		if (s_ReleaseQueueLocked[frameIndex])
		{
			FLUX_CRITICAL_CATEGORY("Renderer", "Recursive call from {0} detected!", functionName);
			FLUX_VERIFY(false);

			// Should we return here or delay the recursive call to the next frame?
			return;
		}

		s_ReleaseQueue[frameIndex].push(std::move(command));
	}
#else
	void Renderer::SubmitRenderCommand(RenderCommand command)
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		s_RenderCommandQueue.push(std::move(command));
	}

	void Renderer::SubmitRenderCommandRelease(RenderCommand command)
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		uint32 frameIndex = Renderer::GetCurrentFrameIndex();
		s_ReleaseQueue[frameIndex].push(std::move(command));
	}
#endif

	void Renderer::FlushRenderCommands()
	{
		FLUX_ASSERT_ON_MAIN_THREAD();
		
#ifndef FLUX_BUILD_SHIPPING
		s_RenderCommandQueueLocked = true;
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

	void Renderer::FlushReleaseQueue(uint32 frameIndex)
	{
		// FLUX_ASSERT_ON_RENDER_THREAD();

#ifndef FLUX_BUILD_SHIPPING
		s_ReleaseQueueLocked[frameIndex] = true;
#endif

		auto& queue = s_ReleaseQueue[frameIndex];
		while (!queue.empty())
		{
			RenderCommand& command = queue.front();
			command();
			queue.pop();
		}

#ifndef FLUX_BUILD_SHIPPING
		s_ReleaseQueueLocked[frameIndex] = false;
#endif

	}

	uint32 Renderer::GetCurrentFrameIndex()
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		return 0; // Engine::Get().GetSwapchain()->GetCurrentBufferIndex()
	}

	uint32 Renderer::RT_GetCurrentFrameIndex()
	{
		// FLUX_ASSERT_ON_RENDER_THREAD();

		return Engine::Get().GetSwapchain()->GetCurrentBufferIndex();
	}

	uint32 Renderer::GetFramesInFlight()
	{
		return 1;
	}

}