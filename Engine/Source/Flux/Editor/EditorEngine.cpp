#include "FluxPCH.h"
#include "EditorEngine.h"

#include "Flux/Runtime/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

	EditorEngine::EditorEngine(const EngineCreateInfo& createInfo)
		: Engine(createInfo)
	{
		CreateWindowMenus();
	}

	EditorEngine::~EditorEngine()
	{
		FLUX_CHECK_IS_IN_EVENT_THREAD();
	}

	void EditorEngine::OnInit()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		OpenProject();
	}

	void EditorEngine::OnShutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		CloseProject();

		FLUX_VERIFY(m_ViewportPlaceholderTexture->GetReferenceCount() == 1);
		m_ViewportPlaceholderTexture = nullptr;

		delete[] m_ViewportPlaceholderTextureData;
	}

	void EditorEngine::OnUpdate()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		// Clear color (TODO: remove)
		FLUX_SUBMIT_RENDER_COMMAND([windowWidth = m_MainWindow->GetWidth(), windowHeight = m_MainWindow->GetHeight()]() mutable
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			glDisable(GL_SCISSOR_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_SCISSOR_TEST);

			glViewport(0, 0, windowWidth, windowHeight);
		});
	}

	void EditorEngine::OnImGuiRender()
	{
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

		if (m_ViewportWidth != viewportWidth || m_ViewportHeight != viewportHeight)
		{
			m_ViewportWidth = viewportWidth;
			m_ViewportHeight = viewportHeight;

			if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			{
				delete[] m_ViewportPlaceholderTextureData;
				uint32 size = m_ViewportWidth * m_ViewportHeight * 4;
				m_ViewportPlaceholderTextureData = new uint8[size];

				for (uint32 i = 0; i < size; i += 4)
				{
					m_ViewportPlaceholderTextureData[i + 0] = (uint8)(((float)i / (float)size) * 86);
					m_ViewportPlaceholderTextureData[i + 1] = (uint8)(((float)i / (float)size) * 157);
					m_ViewportPlaceholderTextureData[i + 2] = (uint8)(((float)i / (float)size) * 233);
					m_ViewportPlaceholderTextureData[i + 3] = 0xFF;
				}

				if (m_ViewportPlaceholderTexture)
				{
					TextureProperties properties = m_ViewportPlaceholderTexture->GetProperties();
					properties.Width = m_ViewportWidth;
					properties.Height = m_ViewportHeight;
					m_ViewportPlaceholderTexture->Reinitialize(properties);

					m_ViewportPlaceholderTexture->SetData(m_ViewportPlaceholderTextureData, m_ViewportWidth * m_ViewportHeight);
					m_ViewportPlaceholderTexture->Apply();
				}
				else
				{
					TextureProperties properties;
					properties.Width = m_ViewportWidth;
					properties.Height = m_ViewportHeight;
					properties.Format = TextureFormat::RGBA32;
					m_ViewportPlaceholderTexture = Texture::Create(properties, m_ViewportPlaceholderTextureData);
				}
			}
		}

		if (m_ViewportPlaceholderTexture)
			m_ImGuiRenderer->Image(m_ViewportPlaceholderTexture, { (float)m_ViewportWidth, (float)m_ViewportHeight });

		ImGui::End();

		ImGui::Begin("Flux Engine");

		BuildConfiguration buildConfig = Engine::GetBuildConfiguration();
		const char* buildConfigString = Utils::BuildConfigurationToString(buildConfig);
		ImGui::Text("%s build", buildConfigString);
		ImGui::Separator();

		ImGui::Text("Ticks per second: %d", m_TicksPerSecond);
		ImGui::Text("Events per second: %d", m_EventsPerSecond);
		ImGui::Separator();

		ImGui::Checkbox("V-Sync", &m_VSync);
		ImGui::Text("%d fps", m_FramesPerSecond);
		ImGui::Text("Delta Time: %.2fms", m_DeltaTime * 1000.0f);
		// ImGui::DragFloat("Fixed Delta Time", &m_FixedDeltaTime, 0.001f, 0.001f, 0.1f);
		ImGui::Text("Fixed Delta Time: %.2f", m_FixedDeltaTime);

		ImGui::Separator();
		ImGui::Text("Command queues: %d", Renderer::GetQueueCount());

		ImGui::BeginDisabled();
		bool multithreaded = m_RenderThread != nullptr;
		ImGui::Checkbox("Multithreaded", &multithreaded);
		ImGui::EndDisabled();

		if (m_RenderThread)
		{
			ImGui::Separator();
			ImGui::Text("Render Thread wait: %.2fms", m_RenderThreadWaitTime);
		}

		ImGui::End();
	}

	void EditorEngine::CreateWindowMenus()
	{
		FLUX_CHECK_IS_IN_EVENT_THREAD();

		WindowMenu fileMenu = m_MainWindow->CreateMenu();
		m_MainWindow->AddMenu(fileMenu, Menu_File_NewProject, "New Project...");
		m_MainWindow->AddMenu(fileMenu, Menu_File_OpenProject, "Open Project...");
		m_MainWindow->AddMenu(fileMenu, Menu_File_SaveProject, "Save Project");
		m_MainWindow->AddMenuSeparator(fileMenu);
		m_MainWindow->AddMenu(fileMenu, Menu_File_Restart, "Restart");
		m_MainWindow->AddMenu(fileMenu, Menu_File_Exit, "Exit\tAlt+F4");

		WindowMenu editMenu = m_MainWindow->CreateMenu();
		m_MainWindow->AddMenu(editMenu, Menu_Edit_Preferences, "Preferences");

		WindowMenu aboutMenu = m_MainWindow->CreateMenu();
		m_MainWindow->AddMenu(aboutMenu, Menu_About_AboutFluxEngine, "About Flux Engine");

		WindowMenu menu = m_MainWindow->CreateMenu();
		m_MainWindow->AddPopupMenu(menu, fileMenu, "File");
		m_MainWindow->AddPopupMenu(menu, editMenu, "Edit");
		m_MainWindow->AddPopupMenu(menu, aboutMenu, "About");

		WindowCreateInfo windowCreateInfo;
		windowCreateInfo.ParentWindow = m_MainWindow;

		// About
		windowCreateInfo.Title = "About Flux Engine";
		windowCreateInfo.Width = 610;
		windowCreateInfo.Height = 410;
		windowCreateInfo.Resizable = false;
		m_Windows[Menu_About_AboutFluxEngine] = Window::Create(windowCreateInfo);

		// Preferences
		windowCreateInfo.Title = "Preferences";
		windowCreateInfo.Width = 960;
		windowCreateInfo.Height = 540;
		windowCreateInfo.Resizable = true;
		m_Windows[Menu_Edit_Preferences] = Window::Create(windowCreateInfo);

		for (auto& [type, window] : m_Windows)
			window->SetEventQueue(m_EventQueue);

		m_MainWindow->SetMenu(menu);
	}

	void EditorEngine::OnEvent(Event& event)
	{
		EventHandler handler(event);
		handler.Bind<WindowMenuEvent>(FLUX_BIND_CALLBACK(OnWindowMenuEvent, this));
		handler.Bind<WindowCloseEvent>(FLUX_BIND_CALLBACK(OnWindowCloseEvent, this));
	}

	void EditorEngine::OnWindowMenuEvent(WindowMenuEvent& event)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		MenuItem item = static_cast<MenuItem>(event.GetItemID());

		SubmitToEventThread([this, item]()
		{
			auto it = m_Windows.find(item);
			if (it != m_Windows.end())
				it->second->SetVisible(true);
		});

		switch (item)
		{
		case Menu_File_NewProject:
		{
			NewProject();
			break;
		}
		case Menu_File_OpenProject:
		{
			OpenProject();
			break;
		}
		case Menu_File_SaveProject:
		{
			SaveProject();
			break;
		}
		case Menu_File_Restart:
		{
			Engine::Get().Close(true);
			break;
		}
		case Menu_File_Exit:
		{
			Engine::Get().Close();
			break;
		}
		}
	}

	void EditorEngine::OnWindowCloseEvent(WindowCloseEvent& event)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		SubmitToEventThread([this, window = event.GetWindow()]()
		{
			for (auto& entry : m_Windows)
			{
				if (entry.second == window)
					window->SetVisible(false);
			}
		});
	}

	void EditorEngine::OpenProject()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		std::string path;
		// while (path.empty())
		Platform::OpenFolderDialog(nullptr, &path, "Load Project");

		OpenProject(path);
	}

	void EditorEngine::OpenProject(const std::filesystem::path& path)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (!std::filesystem::exists(path))
			return;

		if (m_Project && path == m_Project->GetProjectDirectory())
			return;

		CloseProject();

		FLUX_INFO("Opening project: {0}", path.filename().stem().string());

		m_Project = Project::Load(path);
	}

	void EditorEngine::NewProject()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		std::string path;
		Platform::OpenFolderDialog(nullptr, &path, "Choose location for new project");

		if (!path.empty())
		{
			CloseProject();

			// m_Project = Ref<Project>::Create(path + '/' + ...)
		}
	}

	void EditorEngine::SaveProject()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (m_Project)
		{
			FLUX_INFO("Saving project: {0}", m_Project->GetName());

			m_Project->SaveSettings();
		}
	}

	void EditorEngine::CloseProject()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (m_Project)
		{
			FLUX_INFO("Saving and closing project: {0}", m_Project->GetName());

			m_Project->SaveSettings();

			FLUX_VERIFY(m_Project->GetReferenceCount() == 1);
			m_Project = nullptr;
		}
	}

}