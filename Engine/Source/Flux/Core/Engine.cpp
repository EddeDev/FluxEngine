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

			// Initialize ImGui
			// m_ImGuiRenderer = Ref<ImGuiRenderer>::Create(m_Window->GetNativeHandle());

			// if (m_ImGuiRenderer)
			// 	m_ImGuiRenderer->InitResources();
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
			-0.5f,  0.5f, 0.0f, 0.8, 0.2, 0.2,
			-0.5f, -0.5f, 0.0f, 0.8, 0.8, 0.2,
			 0.5f, -0.5f, 0.0f, 0.2, 0.8, 0.8,
			 0.5f,  0.5f, 0.0f, 0.8, 0.2, 0.8
		};

		uint32 indices[] = {
			0, 1, 3,
			3, 1, 2
		};

		m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		m_IndexBuffer = IndexBuffer::Create(indices, sizeof(indices));

		static const char* s_VertexShaderSource =
			"#version 450 core\n"
			"layout(location = 0) in vec3 a_Position;\n"
			"layout(location = 1) in vec3 a_Color;\n"
			"layout(location = 0) out vec3 v_Color;\n"
			"void main()\n"
			"{\n"
			"    v_Color = a_Color;\n"
			"    gl_Position = vec4(a_Position, 1.0);\n"
			"}\n";

		static const char* s_FragmentShaderSource =
			"#version 450 core\n"
			"layout(location = 0) out vec4 o_Color;\n"
			"layout(location = 0) in vec3 v_Color;\n"
			"void main()\n"
			"{\n"
			"    o_Color = vec4(v_Color, 1.0);\n"
			"}\n";

		m_Shader = Shader::Create(s_VertexShaderSource, s_FragmentShaderSource);

		FLUX_SUBMIT_RENDER_COMMAND([this]()
		{
			glCreateVertexArrays(1, &m_VertexArrayID);
		});

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

			// Clear color (TODO: remove)
			FLUX_SUBMIT_RENDER_COMMAND([]() mutable
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			});

			m_VertexBuffer->Bind();

			FLUX_SUBMIT_RENDER_COMMAND([this]() mutable
			{
				glBindVertexArray(m_VertexArrayID);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, 0);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (uintptr*)12);
				glEnableVertexAttribArray(1);
			});

			m_IndexBuffer->Bind();

			m_Shader->Bind();

			FLUX_SUBMIT_RENDER_COMMAND([]() mutable
			{
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
			});

			FLUX_SUBMIT_RENDER_COMMAND([this]() mutable
			{
				if (m_ImGuiRenderer)
				{
					m_ImGuiRenderer->BeginFrame();
					ImGui::ShowDemoWindow();
					m_ImGuiRenderer->EndFrame();
				}
			});
			
			// Wait for the previous frame to finish
			m_RenderThread->Wait();

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

			FLUX_VERIFY(m_IndexBuffer->GetReferenceCount() == 1);
			m_IndexBuffer = nullptr;

			FLUX_VERIFY(m_Shader->GetReferenceCount() == 1);
			m_Shader = nullptr;
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