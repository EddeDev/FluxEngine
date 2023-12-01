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

		WindowMenu fileMenu = m_Window->CreateMenu();
		m_Window->AddMenu(fileMenu, Menu_File_NewProject, "New Project...", true);
		m_Window->AddMenu(fileMenu, Menu_File_OpenProject, "Open Project...");
		m_Window->AddMenu(fileMenu, Menu_File_SaveProject, "Save Project", true);
		m_Window->AddMenuSeparator(fileMenu);
		m_Window->AddMenu(fileMenu, Menu_File_Restart, "Restart");
		m_Window->AddMenu(fileMenu, Menu_File_Exit, "Exit\tAlt+F4");

		WindowMenu editMenu = m_Window->CreateMenu();
		m_Window->AddMenu(editMenu, Menu_Edit_Preferences, "Preferences");

		WindowMenu aboutMenu = m_Window->CreateMenu();
		m_Window->AddMenu(aboutMenu, Menu_About_AboutFluxEngine, "About Flux Engine");

		WindowMenu menu = m_Window->CreateMenu();
		m_Window->AddPopupMenu(menu, fileMenu, "File");
		m_Window->AddPopupMenu(menu, editMenu, "Edit");
		m_Window->AddPopupMenu(menu, aboutMenu, "About");

		m_Window->SetMenu(menu);
		m_Window->AddMenuCallback(FLUX_BIND_CALLBACK(OnMenuCallback, this));
	}

	Engine::~Engine()
	{
		FLUX_CHECK_IS_EVENT_THREAD();

		s_Instance = nullptr;

		g_EngineRunning = m_RestartOnClose;
	}

	void Engine::Run()
	{
		FLUX_CHECK_IS_EVENT_THREAD();

		ThreadCreateInfo mainThreadCreateInfo;
		mainThreadCreateInfo.Name = "Main Thread";
		mainThreadCreateInfo.Priority = ThreadPriority::Highest;
		m_MainThread = Thread::Create(mainThreadCreateInfo);

		ThreadCreateInfo renderThreadCreateInfo;
		renderThreadCreateInfo.Name = "Render Thread";
		renderThreadCreateInfo.Priority = ThreadPriority::Normal;
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
		FLUX_CHECK_IS_MAIN_THREAD();

		Renderer::Init();
		Input::Init();

		// Initialize ImGui
		m_ImGuiRenderer = Ref<ImGuiRenderer>::Create();

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
				m_FramesPerSecond = m_FrameCounter;
				m_FrameCounter = 0;
				m_LastTime = m_Time;
			}

			Utils::ExecuteQueue(m_MainThreadQueue, m_MainThreadMutex);

			Renderer::BeginFrame();

			Input::Update();

			if (m_ImGuiRenderer)
			{
				m_ImGuiRenderer->NewFrame();

				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

				ImGuiWindowFlags dockspaceFlags = 0;
				dockspaceFlags |= ImGuiWindowFlags_NoDocking;
				dockspaceFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
				dockspaceFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				dockspaceFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

				static bool open = true;
				ImGui::Begin("MainDockspace", &open, dockspaceFlags);
				ImGui::PopStyleVar(3);

				ImGuiIO& io = ImGui::GetIO();
				if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
				{
					ImGuiID dockspaceID = ImGui::GetID("MainDockspace");
					ImGui::DockSpace(dockspaceID);
				}

				ImGui::End();

				ImGui::Begin("Properties");
				ImGui::End();

				ImGui::Begin("Viewport");

				int32 viewportWidth = (int32)ImGui::GetContentRegionAvail().x;
				int32 viewportHeight = (int32)ImGui::GetContentRegionAvail().y;

				if (viewportWidth < 0)
					viewportWidth = 0;
				if (viewportHeight < 0)
					viewportHeight = 0;

				if (m_ViewportWidth != viewportWidth || m_ViewportHeight != m_ViewportHeight)
				{
					m_ViewportWidth = viewportWidth;
					m_ViewportHeight = viewportHeight;

					if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
					{
						delete[] m_ViewportPlaceholderTextureData;
						m_ViewportPlaceholderTextureData = new uint8[m_ViewportWidth * m_ViewportHeight * 4];

						m_ViewportPlaceholderTexture = Texture2D::Create(m_ViewportWidth, m_ViewportHeight, TextureFormat::RGBA32);

						uint32 size = m_ViewportWidth * m_ViewportHeight * 4;

						for (uint32 i = 0; i < m_ViewportWidth * m_ViewportHeight * 4; i += 4)
						{
							m_ViewportPlaceholderTextureData[i + 0] = (uint8)(((float)i / (float)size) * 86);
							m_ViewportPlaceholderTextureData[i + 1] = (uint8)(((float)i / (float)size) * 157);
							m_ViewportPlaceholderTextureData[i + 2] = (uint8)(((float)i / (float)size) * 233);
							m_ViewportPlaceholderTextureData[i + 3] = 0xFF;
						}

						m_ViewportPlaceholderTexture->SetPixelData(m_ViewportPlaceholderTextureData, m_ViewportWidth * m_ViewportHeight);
					}
				}

				if (m_ViewportPlaceholderTexture)
					m_ImGuiRenderer->Image(m_ViewportPlaceholderTexture, { (float)m_ViewportWidth, (float)m_ViewportHeight });
				
				ImGui::End();

				ImGui::Begin("Flux Engine");
				ImGui::Checkbox("V-Sync", &m_VSync);
				ImGui::Text("%d fps", m_FramesPerSecond);
				ImGui::Text("Frame Time: %.2fms", m_FrameTime * 1000.0f);
				ImGui::Text("Render Thread wait: %.2fms", m_RenderThreadWaitTime);
				ImGui::Text("CPU: %.2fms", m_FrameTime * 1000.0f - m_RenderThreadWaitTime);
				ImGui::Text("Command queues: %d", Renderer::GetQueueCount());
				ImGui::End();
			}

			// Clear color (TODO: remove)
			FLUX_SUBMIT_RENDER_COMMAND([windowWidth = m_Window->GetWidth(), windowHeight = m_Window->GetHeight()]() mutable
			{
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

				glDisable(GL_SCISSOR_TEST);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glEnable(GL_SCISSOR_TEST);

				glViewport(0, 0, windowWidth, windowHeight);
			});

			if (m_ImGuiRenderer)
				m_ImGuiRenderer->Render();

			// Wait for the previous frame to finish
			if (Renderer::GetQueueCount() > 1)
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
			m_RenderThread->Submit([queueIndex = Renderer::GetCurrentQueueIndex()]()
			{
				Renderer::FlushRenderCommands(queueIndex);
			});

			if (Renderer::GetQueueCount() == 1)
			{
				uint64 start = Platform::GetNanoTime();
				m_RenderThread->Wait();
				uint64 end = Platform::GetNanoTime();

				m_RenderThreadWaitTime = float(end - start) * 0.001f * 0.001f;
			}

			Renderer::EndFrame();
		}

		// destroy resources
		{
			FLUX_VERIFY(m_ImGuiRenderer->GetReferenceCount() == 1);
			m_ImGuiRenderer = nullptr;
			
			FLUX_VERIFY(m_ViewportPlaceholderTexture->GetReferenceCount() == 1);
			m_ViewportPlaceholderTexture = nullptr;

			delete[] m_ViewportPlaceholderTextureData;
		}

		m_RenderThread->Submit([queueIndex = Renderer::GetCurrentQueueIndex()]()
		{
			Renderer::FlushRenderCommands(queueIndex);
			Renderer::FlushReleaseQueue();
		});
		m_RenderThread->Wait();

		Input::Shutdown();
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

	void Engine::OnMenuCallback(WindowMenu menu, uint32 menuID)
	{
		FLUX_CHECK_IS_EVENT_THREAD();

		switch (menuID)
		{
		case Menu_File_OpenProject:
		{
			std::string outPath;
			if (Platform::OpenFolderDialog(m_Window.get(), &outPath, "Load Project") == DialogResult::Ok)
				FLUX_ERROR("Path: {0}", outPath);
			break;
		}
		case Menu_File_Restart:
		{
			Engine::Get().SubmitToMainThread([this]() { Engine::Get().Close(true); });
			break;
		}
		case Menu_File_Exit:
		{
			Engine::Get().SubmitToMainThread([this]() { Engine::Get().Close(); });
			break;
		}
		default:
		{
			auto it = m_Windows.find(static_cast<MenuItem>(menuID));
			if (it != m_Windows.end())
				it->second->SetVisible(true);
			break;
		}
		}
	}

}