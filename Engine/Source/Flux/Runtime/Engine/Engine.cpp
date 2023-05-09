#include "FluxPCH.h"
#include "Engine.h"

#include "Flux/Runtime/Core/Platform.h"
#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	extern bool g_EngineRunning;

	Engine::Engine()
	{
		FLUX_ASSERT(!s_Instance, "Engine instance already exists!");
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
		FLUX_ASSERT(s_Instance, "Engine instance is nullptr!");
		s_Instance = nullptr;
	}

	void Engine::Run()
	{
		OnInit();

		while (m_Running)
		{
			float time = Platform::GetTime();
			m_FrameTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			m_FrameCounter++;
			if (time >= m_LastTime + 1.0f)
			{
				m_FramesPerSecond = m_FrameCounter;
				m_FrameCounter = 0;
				m_LastTime = time;
			}

			Platform::GetNanoTime();

			FLUX_WARNING("Frame time: {0:.1f}ms ({1} fps)", m_FrameTime * 1000.0f, m_FramesPerSecond);

			OnUpdate();

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