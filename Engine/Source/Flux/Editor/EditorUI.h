#pragma once

namespace Flux {

	namespace UI {

		class Renderer
		{

		};

		void Init(bool swapchainTarget = false);
		void Shutdown();

		void BeginFrame();
		void EndFrame();
		
	}

}