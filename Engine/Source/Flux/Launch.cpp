#include "FluxPCH.h"

#include "Runtime/Core/Engine.h"

#define FLUX_EDITOR
// #define FLUX_RUNTIME

#ifdef FLUX_EDITOR
	#include "Editor/EditorEngine.h"
#endif

#ifdef FLUX_RUNTIME
	#include "Runtime/Core/RuntimeEngine.h"
#endif

namespace Flux {

	bool g_EngineRunning = true;

	Engine* CreateEngine()
	{
		EngineCreateInfo createInfo;

#ifdef FLUX_EDITOR
		createInfo.Title = "Flux Editor";
		createInfo.EnableImGui = true;
		createInfo.ClearImGuiPass = true;
		createInfo.ShowSplashScreen = true;
		createInfo.MaximizeOnStart = true;
		createInfo.Multithreaded = true;

		// Disable V-Sync
		createInfo.VSync = false;

		return new EditorEngine(createInfo);
#endif

#ifdef FLUX_RUNTIME
		createInfo.Title = "Flux Runtime";
		createInfo.EnableImGui = false;
		createInfo.ClearImGuiPass = false;
		createInfo.ShowSplashScreen = false;
		createInfo.MaximizeOnStart = false;
		createInfo.Multithreaded = true;
		createInfo.VSync = false;

		return new RuntimeEngine(createInfo);
#endif

		return nullptr;
	}

	int32 Main()
	{
		while (g_EngineRunning)
		{
			Logger::Init();
			Platform::Init();

			FLUX_INFO("Initializing...");

			Engine* engine = CreateEngine();
			FLUX_VERIFY(engine);

			if (engine)
			{
				engine->Run();
				delete engine;
			}

			FLUX_INFO("Shutting down...");

			Platform::Shutdown();
			Logger::Shutdown();
		}

		return 0;
	}

}