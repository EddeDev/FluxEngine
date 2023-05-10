#include "FluxPCH.h"

#define FLUX_EDITOR

#ifdef FLUX_EDITOR
	#include "Flux/Editor/EditorEngine.h"
#endif

namespace Flux {

	bool g_EngineRunning = true;

	Engine* CreateEngine()
	{
#ifdef FLUX_EDITOR
		return new EditorEngine();
#endif

		return nullptr;
	}

	int32 Main()
	{
		while (g_EngineRunning)
		{
			Logger::Init();
			Platform::Init();

			FLUX_INFO("Flux Engine");
			FLUX_INFO("Initializing...");

			Engine* engine = CreateEngine();
			if (engine)
				engine->Run();
			else
				FLUX_VERIFY(false, "Engine is nullptr");

			FLUX_INFO("Shutting down...");
			delete engine;

			Platform::Shutdown();
			Logger::Shutdown();
		}

		return 0;
	}

}