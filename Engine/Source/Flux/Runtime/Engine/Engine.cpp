#include "FluxPCH.h"
#include "Engine.h"

#include "Flux/Runtime/Core/Platform.h"
#include "Flux/Runtime/Core/Window.h"

namespace Flux {

	namespace Utils {

		static void ExecuteQueue(std::queue<std::function<void()>>& queue, std::mutex& mutex)
		{
			std::unique_lock<std::mutex> lock(mutex);
			while (!queue.empty())
			{
				auto& callback = queue.front();
				callback();
				queue.pop();
			}
		}
	}

	extern bool g_EngineRunning;

	Engine::Engine()
	{
		FLUX_ASSERT(!s_Instance, "Engine instance already exists!");
		s_Instance = this;

		m_EventThreadID = Platform::GetCurrentThreadID();

		Platform::SetConsoleTitle("Flux Engine");
		Platform::SetThreadName(Platform::GetCurrentThread(), "Event Thread");
		Platform::SetThreadPriority(Platform::GetCurrentThread(), ThreadPriority::Lowest);

		WindowCreateInfo windowCreateInfo;
		windowCreateInfo.Width = 1280;
		windowCreateInfo.Height = 720;

		m_Window = Window::Create(windowCreateInfo);
		m_Window->AddCloseCallback(FLUX_BIND_CALLBACK(OnWindowClose, this));

		Ref<GraphicsContext> context = GraphicsContext::Create();
	}

	Engine::~Engine()
	{
		s_Instance = nullptr;
	}

	void Engine::Run()
	{
		ThreadCreateInfo mainThreadCreateInfo;
		mainThreadCreateInfo.Name = "Main Thread";
		mainThreadCreateInfo.Priority = ThreadPriority::Highest;

		m_MainThread = Thread::Create(mainThreadCreateInfo);
		m_MainThreadID = m_MainThread->GetID();

		m_MainThread->Submit(FLUX_BIND_CALLBACK(MT_MainLoop, this));

		while (m_Running)
		{
			Platform::WaitMessage();
			Platform::PumpMessages();

			Utils::ExecuteQueue(m_EventThreadQueue, m_EventThreadMutex);
		}

		m_MainThread.reset();
	}

	void Engine::MT_MainLoop()
	{
		OnInit();

		SubmitToEventThread([this]()
		{
			m_Window->SetVisible(true);
		});

		while (m_Running)
		{
			float time = Platform::GetTime();
			m_FrameTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			m_FrameCounter++;
			if (time >= m_LastTime + 1.0f)
			{
				FLUX_TRACE("Frame time: {0:.2f}ms ({1} fps)", m_FrameTime * 1000.0f, m_FrameCounter);

				m_FramesPerSecond = m_FrameCounter;
				m_FrameCounter = 0;
				m_LastTime = time;
			}

			Utils::ExecuteQueue(m_MainThreadQueue, m_MainThreadMutex);

			OnUpdate();

			Platform::Sleep(0.001f);
		}

		OnExit();
	}

	void Engine::Close(bool restart)
	{
		FLUX_ASSERT_ON_THREAD(m_MainThreadID);

		m_Running = false;
		g_EngineRunning = restart;
	}

	void Engine::OnWindowClose()
	{
		SubmitToMainThread([this]()
		{
			Close();
		});
	}

	void Engine::SubmitToEventThread(std::function<void()> function)
	{
		std::lock_guard<std::mutex> lock(m_EventThreadMutex);
		m_EventThreadQueue.push(std::move(function));
		Platform::PostEmptyEvent();
	}

	void Engine::SubmitToMainThread(std::function<void()> function)
	{
		std::lock_guard<std::mutex> lock(m_MainThreadMutex);
		m_MainThreadQueue.push(std::move(function));
	}

}