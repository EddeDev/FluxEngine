#pragma once

namespace Flux {

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void BeginRenderPass();
		static void EndRenderPass();
	};

}