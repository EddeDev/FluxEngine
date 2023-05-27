#pragma once

namespace Flux {

	namespace UI {

		void Init(bool swapchainTarget = false);
		void Shutdown();

		void BeginFrame();
		void EndFrame();
		
	}

}