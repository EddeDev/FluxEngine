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

			Engine* engine = new Engine();
			engine->Run();
			delete engine;

			Platform::Shutdown();
			Logger::Shutdown();
		}
	
		return 0;
	}

}