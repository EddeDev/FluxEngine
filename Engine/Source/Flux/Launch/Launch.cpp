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

		auto menu = WindowMenu::Create();
		{
			auto fileMenu = menu->AddMenu("File");
			fileMenu->AddMenu("Open Scene");
			fileMenu->AddSeparator();
			fileMenu->AddMenu("Exit", [&]() { running = false; });

			auto helpMenu = menu->AddMenu("Help");
			helpMenu->AddMenu("About");
		}
		window->SetMenu(menu);

		while (running)
		{
			window->ProcessEvents();
		}

		FLUX_INFO("Shutting down...");
		Logger::Shutdown();
		return 0;
	}

}