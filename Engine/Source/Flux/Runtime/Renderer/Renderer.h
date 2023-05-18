#pragma once

#include "CommandBuffer.h"
#include "Framebuffer.h"

namespace Flux {

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void BeginRenderPass(Ref<CommandBuffer> commandBuffer, Ref<Framebuffer> framebuffer);
		static void EndRenderPass(Ref<CommandBuffer> commandBuffer);

		static uint32 GetCurrentFrameIndex();
	};

}