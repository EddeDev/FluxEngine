#include "FluxPCH.h"
#include "Engine.h"

#include "Flux/Runtime/Renderer/Renderer.h"

namespace Flux {

	namespace Utils {

		static void ExecuteQueue(std::queue<std::function<void()>>& queue, std::mutex& mutex)
		{
			std::lock_guard<std::mutex> lock(mutex);
			while (!queue.empty())
			{
				auto& callback = queue.front();
				callback();
				queue.pop();
			}
		}
	}

	extern bool g_EngineRunning;

	Engine::Engine(const EngineCreateInfo& createInfo)
		: m_CreateInfo(createInfo)
	{
		FLUX_VERIFY(!s_Instance);
		s_Instance = this;

		m_EventThreadID = Platform::GetCurrentThreadID();

		Platform::SetConsoleTitle("Flux Engine");
		Platform::SetThreadName(Platform::GetCurrentThread(), "Event Thread");
		Platform::SetThreadPriority(Platform::GetCurrentThread(), ThreadPriority::Lowest);

		m_EventQueue = Ref<EventQueue>::Create();
		m_EventQueue->SetEventCallback([this](Event& event)
		{
			FLUX_CHECK_IS_IN_MAIN_THREAD();

			Input::OnEvent(event);

			EventHandler handler(event);
			handler.Bind<WindowCloseEvent>([this](WindowCloseEvent& event)
			{
				if (event.GetWindow() == m_MainWindow)
					Close();
			});
			handler.Bind<WindowMinimizeEvent>([this](WindowMinimizeEvent& event)
			{
				m_Minimized = event.IsMinimized();
			});
			handler.Bind<WindowResizeEvent>([this](WindowResizeEvent& event)
			{
				if (event.GetWidth() == 0 || event.GetHeight() == 0)
				{
					m_Minimized = true;
					return;
				}

				m_Minimized = false;
			});

			OnEvent(event);

			if (m_ImGuiRenderer)
				m_ImGuiRenderer->OnEvent(event);
		});

		WindowCreateInfo windowCreateInfo;
		windowCreateInfo.Width = createInfo.WindowWidth;
		windowCreateInfo.Height = createInfo.WindowHeight;
		m_MainWindow = Window::Create(windowCreateInfo);
		m_MainWindow->SetEventQueue(m_EventQueue);
	}

	Engine::~Engine()
	{
		FLUX_CHECK_IS_IN_EVENT_THREAD();

		s_Instance = nullptr;

		g_EngineRunning = m_RestartOnClose;
	}

	void Engine::Run()
	{
		FLUX_CHECK_IS_IN_EVENT_THREAD();

		ThreadCreateInfo mainThreadCreateInfo;
		mainThreadCreateInfo.Name = "Main Thread";
		mainThreadCreateInfo.Priority = ThreadPriority::Highest;
		m_MainThread = Thread::Create(mainThreadCreateInfo);

		if (m_CreateInfo.Multithreaded)
		{
			ThreadCreateInfo renderThreadCreateInfo;
			renderThreadCreateInfo.Name = "Render Thread";
			renderThreadCreateInfo.Priority = ThreadPriority::Normal;
			m_RenderThread = Thread::Create(renderThreadCreateInfo);
		}

		m_MainThreadID = m_MainThread->GetID();
		m_RenderThreadID = m_RenderThread ? m_RenderThread->GetID() : m_MainThreadID;

		if (m_CreateInfo.Multithreaded)
			m_RenderThread->SubmitAndWait(FLUX_BIND_CALLBACK(CreateRendererContext, this));
		else
			m_MainThread->Submit(FLUX_BIND_CALLBACK(CreateRendererContext, this));

		m_MainThread->Submit(FLUX_BIND_CALLBACK(MainLoop, this));

		while (m_Running)
		{
			Platform::WaitMessage();
			Platform::PumpMessages();

			Utils::ExecuteQueue(m_EventThreadQueue, m_EventThreadMutex);
		}

		m_MainThread.reset();

		if (m_RenderThread)
		{
			m_RenderThread->Submit(FLUX_BIND_CALLBACK(DestroyRendererContext, this));
			m_RenderThread.reset();
		}
		else
		{
			m_MainThread->Submit(FLUX_BIND_CALLBACK(DestroyRendererContext, this));
		}
	}

	void Engine::CreateRendererContext()
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();

		m_Context = GraphicsContext::Create(m_MainWindow->GetNativeHandle());
		m_Context->Init();
	}

	void Engine::DestroyRendererContext()
	{
		FLUX_CHECK_IS_IN_RENDER_THREAD();

		FLUX_VERIFY(m_Context->GetReferenceCount() == 1);
		m_Context = nullptr;
	}

	void Engine::Close(bool restart)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Running = false;
		m_RestartOnClose = restart;
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

	void Engine::MainLoop()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		Renderer::Init(m_RenderThread ? 2 : 1);
		Input::Init();

		if (m_CreateInfo.EnableImGui)
		{
			// Initialize ImGui
			m_ImGuiRenderer = Ref<ImGuiRenderer>::Create();
		}

		OnInit();

		// Show window
		SubmitToEventThread([this]() { m_MainWindow->SetVisible(true); });

		while (m_Running)
		{
			m_CurrentTime = Platform::GetTime();
			m_DeltaTime = Math::Min(m_CurrentTime - m_LastTime, m_MaxDeltaTime);
			m_LastTime = m_CurrentTime;

			m_FrameCounter++;
			m_EventCounter += m_EventQueue->GetEventCount();
			if (m_CurrentTime >= m_LastFrameTime + 1.0f)
			{
				m_FramesPerSecond = m_FrameCounter;
				m_EventsPerSecond = m_EventCounter;
				m_FrameCounter = 0;
				m_EventCounter = 0;
				m_LastFrameTime = m_CurrentTime;
			}

			Utils::ExecuteQueue(m_MainThreadQueue, m_MainThreadMutex);

			Renderer::BeginFrame();

			Input::Update();

			m_EventQueue->DispatchEvents();

			if (!m_Minimized)
			{
				OnUpdate(m_DeltaTime);

				if (m_ImGuiRenderer)
				{
					m_ImGuiRenderer->NewFrame();
					OnImGuiRender();
					m_ImGuiRenderer->Render();
				}
			}

			// Wait for the previous frame to finish
			if (m_RenderThread && Renderer::GetQueueCount() > 1)
			{
				uint64 start = Platform::GetNanoTime();
				m_RenderThread->Wait();
				uint64 end = Platform::GetNanoTime();

				m_RenderThreadWaitTime = float(end - start) * 0.001f * 0.001f;
			}

			// Swap buffers
			FLUX_SUBMIT_RENDER_COMMAND([context = m_Context, vsync = m_VSync]() mutable
			{
				context->SwapBuffers(vsync ? 1 : 0);
			});

			// Flush render command queue
			uint32 queueIndex = Renderer::GetCurrentQueueIndex();
			if (m_RenderThread)
			{
				m_RenderThread->Submit([queueIndex]()
				{
					Renderer::FlushRenderCommands(queueIndex);
				});
			}
			else
			{
				Renderer::FlushRenderCommands(queueIndex);
			}

			if (m_RenderThread && Renderer::GetQueueCount() == 1)
			{
				uint64 start = Platform::GetNanoTime();
				m_RenderThread->Wait();
				uint64 end = Platform::GetNanoTime();

				m_RenderThreadWaitTime = float(end - start) * 0.001f * 0.001f;
			}

			Renderer::EndFrame();
		}

		OnShutdown();

		if (m_ImGuiRenderer)
		{
			FLUX_VERIFY(m_ImGuiRenderer->GetReferenceCount() == 1);
			m_ImGuiRenderer = nullptr;
		}

		uint32 queueIndex = Renderer::GetCurrentQueueIndex();
		if (m_RenderThread)
		{
			m_RenderThread->SubmitAndWait([queueIndex]()
			{
				Renderer::FlushRenderCommands(queueIndex);
				Renderer::FlushReleaseQueue();
			});
		}
		else
		{
			Renderer::FlushRenderCommands(queueIndex);
			Renderer::FlushReleaseQueue();
		}

		Input::Shutdown();
		Renderer::Shutdown();
	}

	BuildConfiguration Engine::GetBuildConfiguration()
	{
#if defined(FLUX_BUILD_DEBUG)
		return BuildConfiguration::Debug;
#elif defined(FLUX_BUILD_RELEASE)
		return BuildConfiguration::Release;
#elif defined(FLUX_BUILD_SHIPPING)
		return BuildConfiguration::Shipping;
#else
		static_assert(false);
		return BuildConfiguration::None;
#endif
	}

}