#include "FluxPCH.h"
#include "Engine.h"

#include "Platform.h"

#include "Flux/Runtime/Renderer/Renderer.h"

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
		m_Window->AddSizeCallback(FLUX_BIND_CALLBACK(OnWindowResize, this));
		m_Window->AddMinimizeCallback(FLUX_BIND_CALLBACK(OnWindowMinimize, this));

		FLUX_INFO("Graphics API: {0}", GraphicsAPIUtils::ToString(m_GraphicsAPI));
	}

	Engine::~Engine()
	{
		FLUX_ASSERT_IS_EVENT_THREAD();

		s_Instance = nullptr;
	}

	void Engine::Run()
	{
		FLUX_ASSERT_IS_EVENT_THREAD();

		ThreadCreateInfo mainThreadCreateInfo;
		mainThreadCreateInfo.Name = "Main Thread";
		mainThreadCreateInfo.Priority = ThreadPriority::AboveNormal;
		m_MainThread = Thread::Create(mainThreadCreateInfo);

		ThreadCreateInfo renderThreadCreateInfo;
		renderThreadCreateInfo.Name = "Render Thread";
		renderThreadCreateInfo.Priority = ThreadPriority::Highest;
		m_RenderThread = Thread::Create(renderThreadCreateInfo);

		m_MainThreadID = m_MainThread->GetID();
		m_RenderThreadID = m_RenderThread->GetID();

		m_RenderThread->Submit(FLUX_BIND_CALLBACK(RT_Initialize, this));
		m_RenderThread->Wait();

		m_MainThread->Submit(Renderer::Init);

		m_Running = true;
		m_MainThread->Submit(FLUX_BIND_CALLBACK(MT_MainLoop, this));

		while (m_Running)
		{
			Platform::WaitMessage();
			Platform::PumpMessages();

			Utils::ExecuteQueue(m_EventThreadQueue, m_EventThreadMutex);
		}

		m_MainThread.reset();

		m_RenderThread->Submit(FLUX_BIND_CALLBACK(RT_Shutdown, this));
		m_RenderThread.reset();

		if (m_Application)
		{
			delete m_Application;
			m_Application = nullptr;
		}
	}

	void Engine::RT_Initialize()
	{
		m_Context = GraphicsContext::Create();
		m_Adapter = GraphicsAdapter::Create(m_Context);
		m_Device = GraphicsDevice::Create(m_Adapter);
		m_Swapchain = Swapchain::Create(m_Window.get());
	}

	void Engine::RT_Shutdown()
	{
		FLUX_VERIFY(m_Swapchain->GetReferenceCount() == 1);
		m_Swapchain = nullptr;

		FLUX_VERIFY(m_Device->GetReferenceCount() == 1);
		m_Device = nullptr;

		FLUX_VERIFY(m_Adapter->GetReferenceCount() == 1);
		m_Adapter = nullptr;

		FLUX_VERIFY(m_Context->GetReferenceCount() == 1);
		m_Context = nullptr;
	}

	void Engine::MT_MainLoop()
	{
		FLUX_ASSERT_IS_MAIN_THREAD();

		if (m_Application)
			m_Application->OnInit();

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
				FLUX_TRACE("Frame time: {0:.2f}ms, WaitForRenderThread: {1:.2f}ms, ApplicationUpdate: {2:.2f}ms ({3} fps)", m_FrameTime * 1000.0f, m_PerformanceTimers.WaitForRenderThread, m_PerformanceTimers.ApplicationUpdate, m_FrameCounter);

				m_FramesPerSecond = m_FrameCounter;
				m_FrameCounter = 0;
				m_LastTime = time;
			}

			Utils::ExecuteQueue(m_MainThreadQueue, m_MainThreadMutex);

			if (!m_Minimized)
				MT_UpdateAndRender();
			else
				Platform::Sleep(0.2f);
		}

		m_RenderThread->Wait();

		m_RenderThread->Submit([queueIndex = Renderer::GetCurrentQueueIndex()]()
		{
			Renderer::RT_FlushRenderCommands(queueIndex);
		});

		if (m_Application)
			m_Application->OnExit();

		Renderer::DestroyResources();

		m_RenderThread->Submit(Renderer::RT_FlushReleaseQueues);
		m_RenderThread->Wait();

		Renderer::Shutdown();
	}

	void Engine::MT_UpdateAndRender()
	{
		FLUX_ASSERT_IS_MAIN_THREAD();

		FLUX_SUBMIT_RENDER_COMMAND([swapchain = m_Swapchain]() mutable
		{
			Renderer::RT_FlushReleaseQueue(Renderer::RT_GetCurrentFrameIndex());

			swapchain->BeginFrame();
		});

		Renderer::BeginFrame();
		
		Timer timer(TimeUnit::Milliseconds);

		if (m_Application)
		{
			timer.Reset();
			m_Application->OnUpdate();
			m_PerformanceTimers.ApplicationUpdate = timer.GetTime();

			// TODO: Input
		}

		// Wait for the previous frame to finish
		{
			timer.Reset();
			m_RenderThread->Wait();
			m_PerformanceTimers.WaitForRenderThread = timer.GetTime();
		}

		// Swap buffers
		FLUX_SUBMIT_RENDER_COMMAND([swapchain = m_Swapchain, vsync = m_VSync]() mutable
		{
			swapchain->Present(vsync ? 1 : 0);
		});

		m_RenderThread->Submit([queueIndex = Renderer::GetCurrentQueueIndex()]()
		{
			Renderer::RT_FlushRenderCommands(queueIndex);
		});

		Renderer::EndFrame();
	}

	void Engine::Close(bool restart)
	{
		FLUX_ASSERT_IS_MAIN_THREAD();

		m_Running = false;
		g_EngineRunning = restart;
	}

	void Engine::OnWindowClose()
	{
		FLUX_ASSERT_IS_EVENT_THREAD();

		SubmitToMainThread([this]()
		{
			Close();
		});
	}

	void Engine::OnWindowResize(uint32 width, uint32 height)
	{
		FLUX_ASSERT_IS_EVENT_THREAD();

		if (width == 0 || height == 0)
		{
			m_Minimized = true;
			return;
		}

		m_Minimized = false;
	}

	void Engine::OnWindowMinimize(bool minimized)
	{
		FLUX_ASSERT_IS_EVENT_THREAD();

		m_Minimized = minimized;
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