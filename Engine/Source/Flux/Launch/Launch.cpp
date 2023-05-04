#include "FluxPCH.h"

#include "Flux/Core/Window/Window.h"

namespace Flux {

	int32 Main()
	{
		Logger::Init();
		FLUX_INFO("Flux Engine");
		FLUX_INFO("Initializing...");

		WindowCreateInfo windowCreateInfo;
		windowCreateInfo.Width = 1600;
		windowCreateInfo.Height = 900;

		Unique<Window> window = Window::Create(windowCreateInfo);

		bool running = true;
		window->AddCloseCallback([&]()
		{
			running = false;
		});

		while (running)
		{
			window->ProcessEvents();
		}

		FLUX_INFO("Shutting down...");
		Logger::Shutdown();
		return 0;
	}

}