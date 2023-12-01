#include "FluxPCH.h"

#include "Engine.h"

namespace Flux {

	bool g_EngineRunning = true;

	int32 Main()
	{
		while (g_EngineRunning)
		{
			Logger::Init();
			Platform::Init();

			FLUX_INFO("Initializing...");

			Engine* engine = new Engine();
			engine->Run();
			delete engine;

			FLUX_INFO("Shutting down...");

			Platform::Shutdown();
			Logger::Shutdown();
		}

		return 0;
	}

}