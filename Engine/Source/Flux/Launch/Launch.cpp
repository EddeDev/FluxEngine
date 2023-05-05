#include "FluxPCH.h"

#include "Flux/Core/Window/Window.h"
#include "Flux/Core/Platform.h"

namespace Flux {

	int32 Main()
	{
		Logger::Init();
		Platform::Init();

		FLUX_INFO("Flux Engine");
		FLUX_INFO("Initializing...");

		WindowCreateInfo windowCreateInfo;
		windowCreateInfo.Width = 1600 / 2;
		windowCreateInfo.Height = 900 / 2;

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

		std::string msg = Platform::GetErrorMessage(Platform::GetLastError());

		FLUX_DEBUG_BREAK();

		while (running)
		{
			window->ProcessEvents();
		}

		FLUX_INFO("Shutting down...");

		Platform::Shutdown();
		Logger::Shutdown();
		return 0;
	}

}