#include "FluxPCH.h"

#include "Flux/Runtime/Core/Engine.h"

#define FLUX_EDITOR 1

#if FLUX_EDITOR
	#include "Flux/Editor/EditorApplication.h"
#endif

namespace Flux {

	bool g_EngineRunning = true;

	int32 Main()
	{
		while (g_EngineRunning)
		{
			Logger::Init();
			Platform::Init();

			FLUX_INFO("Flux Engine");
			FLUX_INFO("Initializing...");

			Engine* engine = new Engine;
#if FLUX_EDITOR
			engine->SetApplication<EditorApplication>();
#endif

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