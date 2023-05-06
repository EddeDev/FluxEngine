#include "FluxPCH.h"
#include "Engine.h"

#include "Flux/Runtime/Core/Platform.h"
#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	extern bool g_EngineRunning;

	static const uint64 s_NsPerSecond = 1000000000;

	Engine::Engine()
	{
		FLUX_ASSERT(!s_Instance);

		s_Instance = this;

		WindowCreateInfo windowCreateInfo;
		windowCreateInfo.Width = 720;
		windowCreateInfo.Height = 450;

		m_Window = Window::Create(windowCreateInfo);
		m_Window->AddCloseCallback([this]()
		{
			Close();
		});

		Shared<WindowMenu> menu = WindowMenu::Create();

		Shared<WindowMenu> fileMenu = menu->AddMenu("File");
		fileMenu->AddMenu("Open Scene");
		fileMenu->AddSeparator();
		fileMenu->AddMenu("Exit", [this]() { Close(); });

		Shared<WindowMenu> helpMenu = menu->AddMenu("Help");
		helpMenu->AddMenu("About");

		m_Window->SetMenu(menu);
	}

	Engine::~Engine()
	{
		FLUX_ASSERT(s_Instance);

		s_Instance = nullptr;
	}

	void Engine::Run()
	{
		OnInit();

		float lastTime = 0.0f;
		while (m_Running)
		{
			float time = Platform::GetTime();
			float deltaTime = time - lastTime;
			lastTime = time;

			FLUX_TRACE("{0} fps", (int32)(1.0f / deltaTime));

			OnUpdate();

			Platform::Sleep(0.000016f);
			Platform::Tick();
		}

		OnExit();
	}

	void Engine::Close(bool restart)
	{
		m_Running = false;
		g_EngineRunning = restart;
	}

}