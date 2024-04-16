#include "FluxPCH.h"
#include "EditorEngine.h"

#include "EditorAssetDatabase.h"

#include "EditorWindow.h"
#include "HierarchyWindow.h"
#include "InspectorWindow.h"
#include "SceneViewWindow.h"
#include "GameViewWindow.h"
#include "ProjectBrowserWindow.h"

#include "Flux/Runtime/Renderer/Renderer.h"

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

		EditorWindowManager::Init();
		EditorWindowManager::AddWindow<HierarchyWindow>("Hierarchy");
		EditorWindowManager::AddWindow<InspectorWindow>("Inspector");
		EditorWindowManager::AddWindow<SceneViewWindow>("Scene");
		EditorWindowManager::AddWindow<GameViewWindow>("Game");
		EditorWindowManager::AddWindow<ProjectBrowserWindow>("Project");

		OpenProject();
	}

	void EditorEngine::OnShutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		EditorWindowManager::Shutdown();

		CloseProject();
	}

	void EditorEngine::OnUpdate()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		EditorWindowManager::OnUpdate();
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

		EditorWindowManager::OnImGuiRender();

#if 0
		ImGui::Begin("Properties");

		ImGui::ColorEdit4("Albedo Color", m_Material.AlbedoColor.GetPointer());

		ImGui::DragFloat("Ambient Multiplier", &m_AmbientMultiplier, 0.001f, 0.0f, 1.0f);

		ImGui::DragFloat3("Light Rotation", m_LightRotation.GetPointer());
		Quaternion lightRotation = Quaternion(m_LightRotation * Math::DegToRad);
		Vector3 lightDirection = lightRotation * Vector3(0.0f, 0.0f, 1.0f);
		ImGui::Text("Light Direction: [%.2f, %.2f, %.2f]", lightDirection.X, lightDirection.Y, lightDirection.Z);

		ImGui::ColorEdit3("Light Color", m_LightColor.GetPointer());

		ImGui::Text("Camera Position: [%.2f, %.2f, %.2f]", m_EditorCamera.GetPosition().X, m_EditorCamera.GetPosition().Y, m_EditorCamera.GetPosition().Z);
		ImGui::Text("Camera Rotation: [%.2f, %.2f, %.2f]", m_EditorCamera.GetRotation().X, m_EditorCamera.GetRotation().Y, m_EditorCamera.GetRotation().Z);

		ImGui::End();
#endif

#define FLUX_ENGINE_WINDOW 1
#if FLUX_ENGINE_WINDOW
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
#endif
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
		EditorWindowManager::OnEvent(event);

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

		// TODO: hack
		SubmitToEventThread([this]()
		{
			std::string path;
			Platform::OpenFolderDialog(nullptr, &path, "Load Project");

			SubmitToMainThread([this, path]()
			{
				OpenProject(path);
			});
		});
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
		m_Project->RegisterAssetDatabase<EditorAssetDatabase>();

		NewScene();
	}

	void EditorEngine::NewProject()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		SubmitToEventThread([this]()
		{
			std::string path;
			Platform::OpenFolderDialog(nullptr, &path, "Choose location for new project");

			SubmitToMainThread([this, path]()
			{
				if (!path.empty())
				{
					CloseProject();

					// TODO
					// m_Project = Ref<Project>::Create(path + '/' + ...)
				}
			});
		});
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

			SaveScene();
			InitScene(nullptr);

			m_Project->GetAssetDatabase<EditorAssetDatabase>()->SaveAssets();
			m_Project->UnregisterAssetDatabase();

			if (m_EditorScene)
			{
				FLUX_VERIFY(m_EditorScene->GetReferenceCount() == 1);
				m_EditorScene = nullptr;
			}

			m_Project->SaveSettings();
			FLUX_VERIFY(m_Project->GetReferenceCount() == 1);
			m_Project = nullptr;
		}
	}

	void EditorEngine::NewScene()
	{
		m_EditorScene = m_Project->GetAssetDatabase<EditorAssetDatabase>()->CreateMemoryAsset<Scene>();

		InitScene(m_EditorScene);
	}

	void EditorEngine::OpenScene()
	{
		SubmitToEventThread([this]()
		{
			std::string path;
			Platform::OpenFolderDialog(nullptr, &path, "Load Scene");

			SubmitToMainThread([this, path]()
			{
				OpenScene(path);
			});
		});
	}

	void EditorEngine::OpenScene(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
			return;

		if (m_EditorScene)
		{
			const auto& metadata = m_Project->GetAssetDatabase()->GetMetadataFromAsset(m_EditorScene);
			if (path == metadata.RelativeAssetPath)
				return;
		}

		FLUX_INFO("Opening scene: {0}", path.filename().stem().string());

		Ref<Scene> newScene = m_Project->GetAssetDatabase()->GetAssetFromPath(path);
		if (newScene)
		{
			if (m_EditorScene)
				SaveScene();

			m_EditorScene = newScene;
			InitScene(m_EditorScene);
		}
	}

	void EditorEngine::InitScene(Ref<Scene> scene)
	{
		// TODO: Update window title

		// TODO: TEMP
		// if (false)
#if 0
		{
			Entity cubeEntity = m_EditorScene->CreateEmpty("Cube");
			cubeEntity.GetComponent<TransformComponent>().SetLocalPosition({ -4.0f, 0.0f, 0.0f });
			cubeEntity.AddComponent<SubmeshComponent>(Mesh::LoadFromFile("Resources/Meshes/Primitives/Cube.gltf"));
			cubeEntity.AddComponent<MeshRendererComponent>();

			Entity planeEntity = m_EditorScene->CreateEmpty("Plane");
			planeEntity.GetComponent<TransformComponent>().SetLocalPosition({ -2.0f, 0.0f, 0.0f });
			planeEntity.AddComponent<SubmeshComponent>(Mesh::LoadFromFile("Resources/Meshes/Primitives/Plane.gltf"));
			planeEntity.AddComponent<MeshRendererComponent>();

			Entity sphereEntity = m_EditorScene->CreateEmpty("Sphere");
			sphereEntity.GetComponent<TransformComponent>().SetLocalPosition({ 0.0f, 0.0f, 0.0f });
			sphereEntity.AddComponent<SubmeshComponent>(Mesh::LoadFromFile("Resources/Meshes/Primitives/Sphere.gltf"));
			sphereEntity.AddComponent<MeshRendererComponent>();

			Entity capsuleEntity = m_EditorScene->CreateEmpty("Capsule");
			capsuleEntity.GetComponent<TransformComponent>().SetLocalPosition({ 2.0f, 0.0f, 0.0f });
			capsuleEntity.AddComponent<SubmeshComponent>(Mesh::LoadFromFile("Resources/Meshes/Primitives/Capsule.gltf"));
			capsuleEntity.AddComponent<MeshRendererComponent>();

			Entity parentEntity = m_EditorScene->CreateEmpty("Parent Test");
			parentEntity.GetComponent<TransformComponent>().SetLocalPosition({ 4.0f, 0.0f, 0.0f });
			parentEntity.AddComponent<SubmeshComponent>(Mesh::LoadFromFile("Resources/Meshes/Primitives/Cube.gltf"));
			parentEntity.AddComponent<MeshRendererComponent>();

			Entity childEntity = m_EditorScene->CreateEmpty("Child Test");
			childEntity.GetComponent<TransformComponent>().SetLocalPosition({ 2.0f, -2.0f, 0.0f });
			childEntity.AddComponent<SubmeshComponent>(Mesh::LoadFromFile("Resources/Meshes/Primitives/Cube.gltf"));
			childEntity.AddComponent<MeshRendererComponent>();
			childEntity.SetParent(parentEntity);

			Entity cameraEntity = m_EditorScene->CreateCamera("Main Camera");
			cameraEntity.GetComponent<TransformComponent>().SetLocalPosition({ 0.0f, 1.0f, -10.0f });

			Entity directionalLightEntity = m_EditorScene->CreateDirectionalLight("Directional Light", { 50.0f, -30.0f, 0.0f });
		}
#else
		{
			Entity cubeEntity = m_EditorScene->CreateEmpty("Cube");
			cubeEntity.GetComponent<TransformComponent>().SetLocalPosition({ -4.0f, 0.0f, 0.0f });

			Ref<Mesh> cubeMeshAsset = m_Project->GetAssetDatabase()->GetAssetFromPath("Assets/Meshes/Primitives/Cube.gltf");
			cubeEntity.AddComponent<SubmeshComponent>(cubeMeshAsset->GetAssetID());
			cubeEntity.AddComponent<MeshRendererComponent>();

			Entity cameraEntity = m_EditorScene->CreateCamera("Main Camera");
			cameraEntity.GetComponent<TransformComponent>().SetLocalPosition({ 0.0f, 1.0f, -10.0f });

			Entity directionalLightEntity = m_EditorScene->CreateDirectionalLight("Directional Light", { 50.0f, -30.0f, 0.0f });
		}
#endif

		EditorWindowManager::SetActiveScene(scene);
	}

	void EditorEngine::SaveScene()
	{
		FLUX_INFO("Saving scene...");

		if (m_Project->GetAssetDatabase()->IsMemoryAsset(m_EditorScene))
			SaveSceneAs();
		else
			m_Project->GetAssetDatabase<EditorAssetDatabase>()->SaveAsset(m_EditorScene);
	}

	void EditorEngine::SaveSceneAs()
	{
		if (m_EditorScene)
		{
			// TODO
		}
	}

}