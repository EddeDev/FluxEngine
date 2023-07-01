#include "FluxPCH.h"
#include "EditorApplication.h"

#include "Flux/Runtime/Core/Engine.h"

namespace Flux {

	EditorApplication::EditorApplication()
		: Application("Flux Editor")
	{
		auto& window = Engine::Get().GetWindow();

		WindowMenu fileMenu = window->CreateMenu();
		window->AddMenu(fileMenu, Menu_File_NewProject, "New Project...", true);
		window->AddMenu(fileMenu, Menu_File_OpenProject, "Open Project...");
		window->AddMenu(fileMenu, Menu_File_SaveProject, "Save Project", true);
		window->AddMenuSeparator(fileMenu);
		window->AddMenu(fileMenu, Menu_File_Restart, "Restart");
		window->AddMenu(fileMenu, Menu_File_Exit, "Exit\tAlt+F4");

		WindowMenu editMenu = window->CreateMenu();
		window->AddMenu(editMenu, Menu_Edit_Preferences, "Preferences");

		WindowMenu aboutMenu = window->CreateMenu();
		window->AddMenu(aboutMenu, Menu_About_AboutFluxEngine, "About Flux Engine");

		WindowMenu menu = window->CreateMenu();
		window->AddPopupMenu(menu, fileMenu, "File");
		window->AddPopupMenu(menu, editMenu, "Edit");
		window->AddPopupMenu(menu, aboutMenu, "About");

		window->SetMenu(menu);
		window->AddMenuCallback(FLUX_BIND_CALLBACK(OnMenuCallback, this));

		WindowCreateInfo windowCreateInfo;
		windowCreateInfo.ParentWindow = window.get();

		// Preferences
		windowCreateInfo.Width = 960;
		windowCreateInfo.Height = 540;
		windowCreateInfo.Title = "Preferences";
		windowCreateInfo.Resizable = true;
		m_Windows[MenuItem::Menu_Edit_Preferences] = Window::Create(windowCreateInfo);

		// About
		windowCreateInfo.Width = 640;
		windowCreateInfo.Height = 410;
		windowCreateInfo.Title = "About Flux Engine";
		windowCreateInfo.Resizable = false;
		m_Windows[MenuItem::Menu_About_AboutFluxEngine] = Window::Create(windowCreateInfo);

		for (auto& [type, window] : m_Windows)
		{
			if (window)
			{
				window->AddCloseCallback([this, type]()
				{
					auto it = m_Windows.find(type);
					if (it != m_Windows.end())
						it->second->SetVisible(false);
				});
			}
		}
	}

	EditorApplication::~EditorApplication()
	{
	}

	void EditorApplication::OnInit()
	{
		m_RenderPipeline = Ref<ForwardRenderPipeline>::Create();
		m_Texture = Ref<Texture2D>::Create("Resources/Textures/Islandox.png");
	}

	void EditorApplication::OnExit()
	{
		FLUX_VERIFY(m_RenderPipeline->GetReferenceCount() == 1);
		m_RenderPipeline = nullptr;

		FLUX_VERIFY(m_Texture->GetReferenceCount() == 1);
		m_Texture = nullptr;
	}

	void EditorApplication::OnUpdate()
	{
		uint32 width = Engine::Get().GetSwapchain()->GetWidth();
		uint32 height = Engine::Get().GetSwapchain()->GetHeight();
		m_RenderPipeline->SetViewportSize(width, height);

		const float nearClip = 0.01f;
		const float farClip = 1000.0f;

		auto& cameraSettings = m_RenderPipeline->GetCameraSettings();
		cameraSettings.ProjectionMatrix = glm::perspective(glm::radians(70.0f), (float)width / (float)height, nearClip, farClip);
		cameraSettings.NearClip = nearClip;
		cameraSettings.FarClip = farClip;

		m_RenderPipeline->BeginRendering2D();

		m_RenderPipeline->DrawQuad({ 0.0f, 0.0f, -2.0f }, { 1.0f, 1.0f }, { 0.8f, 0.4f, 0.2f, 1.0f });

		m_RenderPipeline->EndRendering2D();
	}

	void EditorApplication::OnMenuCallback(WindowMenu menu, uint32 itemID)
	{
		auto& window = Engine::Get().GetWindow();

		switch (itemID)
		{
		case Menu_File_OpenProject:
		{
			std::string outPath;
			if (Platform::OpenFolderDialog(window.get(), &outPath, "Load Project") == DialogResult::Ok)
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
			auto it = m_Windows.find(static_cast<MenuItem>(itemID));
			if (it != m_Windows.end())
				it->second->SetVisible(true);
			break;
		}
		}
	}

}