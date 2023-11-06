#include "FluxPCH.h"
#include "Engine.h"

#include "Flux/Renderer/Renderer.h"

#include <glad/glad.h>

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
		FLUX_VERIFY(!s_Instance);
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
	}

	Engine::~Engine()
	{
		FLUX_CHECK_IS_EVENT_THREAD();

		s_Instance = nullptr;
	}

	void Engine::Run()
	{
		FLUX_CHECK_IS_EVENT_THREAD();

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

		m_RenderThread->Submit([this]()
		{
			m_Context = GraphicsContext::Create(m_Window->GetNativeHandle());
			m_Context->Init();
		});
		m_RenderThread->Wait();

		m_MainThread->Submit(FLUX_BIND_CALLBACK(MainLoop, this));

		while (m_Running)
		{
			Platform::WaitMessage();
			Platform::PumpMessages();

			Utils::ExecuteQueue(m_EventThreadQueue, m_EventThreadMutex);
		}

		m_MainThread.reset();

		m_RenderThread->Submit([this]()
		{
			FLUX_VERIFY(m_Context->GetReferenceCount() == 1);
			m_Context = nullptr;
		});
		m_RenderThread.reset();
	}

	void Engine::Close(bool restart)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_Running = false;
		g_EngineRunning = restart;
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
		FLUX_CHECK_IS_MAIN_THREAD();

		Renderer::Init();

		float vertices[] =
		{
			-0.5f, 0.5f,  0.0f,
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f, 0.5f,  0.0f
		};

		m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));

		// Show window
		SubmitToEventThread([this]() { m_Window->SetVisible(true); });

		while (m_Running)
		{
			m_Time = Platform::GetTime();
			m_FrameTime = m_Time - m_LastFrameTime;
			m_LastFrameTime = m_Time;

			m_FrameCounter++;
			if (m_Time >= m_LastTime + 1.0f)
			{
				FLUX_TRACE("{0} fps{1}", m_FrameCounter, m_VSync ? " (V-Sync)" : "");

				m_FramesPerSecond = m_FrameCounter;
				m_FrameCounter = 0;
				m_LastTime = m_Time;
			}

			Utils::ExecuteQueue(m_MainThreadQueue, m_MainThreadMutex);

			// Flush release queue
			FLUX_SUBMIT_RENDER_COMMAND([]()
			{
				Renderer::FlushReleaseQueue();
			});

			Renderer::BeginFrame();

			// update app
			
			// Wait for the previous frame to finish
			m_RenderThread->Wait();

			// Clear color (TODO: remove)
			FLUX_SUBMIT_RENDER_COMMAND([]() mutable
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			});

			// Swap buffers
			FLUX_SUBMIT_RENDER_COMMAND([context = m_Context, vsync = m_VSync]() mutable
			{
				context->SwapBuffers(vsync);
			});

			// Flush render command queue
			m_RenderThread->Submit([queueIndex = Renderer::GetCurrentQueueIndex()]()
			{
				Renderer::FlushRenderCommands(queueIndex);
			});

			Renderer::EndFrame();
		}

		m_RenderThread->Submit([queueIndex = Renderer::GetCurrentQueueIndex()]()
		{
			Renderer::FlushRenderCommands(queueIndex);
		});

		m_RenderThread->Wait();

		// destroy resources
		
		// TODO
		{
			FLUX_VERIFY(m_VertexBuffer->GetReferenceCount() == 1);
			m_VertexBuffer = nullptr;
		}

		m_RenderThread->Submit([]() { Renderer::FlushReleaseQueue(); });
		m_RenderThread->Wait();

		Renderer::Shutdown();
	}

	void Engine::OnWindowClose()
	{
		FLUX_CHECK_IS_EVENT_THREAD();

		SubmitToMainThread([this]()
		{
			Close();
		});
	}

}