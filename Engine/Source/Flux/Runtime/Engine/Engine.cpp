#include "FluxPCH.h"
#include "Engine.h"

#include "Flux/Runtime/Core/Platform.h"
#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	extern bool g_EngineRunning;

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
			float deltaTime = glm::min<float>(time - lastTime, 1.0f / 30.0f);
			lastTime = time;

			OnUpdate();

			Platform::Sleep(0.01f);
			Platform::PumpMessages();
		}

		OnExit();
	}

	void Engine::Close(bool restart)
	{
		m_Running = false;
		g_EngineRunning = restart;
	}

}