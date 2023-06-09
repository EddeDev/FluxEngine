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
		m_Windows.clear();
	}

	void EditorApplication::OnInit()
	{
		m_RenderPipeline = Ref<ForwardRenderPipeline>::Create();
	}

	void EditorApplication::OnExit()
	{
		FLUX_VERIFY(m_RenderPipeline->GetReferenceCount() == 1);
		m_RenderPipeline = nullptr;
	}

	void EditorApplication::OnUpdate()
	{
		float width = Engine::Get().GetSwapchain()->GetWidth();
		float height = Engine::Get().GetSwapchain()->GetHeight();

		m_RenderPipeline->BeginRendering2D();

		static float rotation = 0.0f;
		rotation += Engine::Get().GetFrameTime() * 10.0f;
		m_RenderPipeline->DrawQuad({ 0.0f, 0.0f, -2.0f }, rotation, { 1.0f, 1.0f }, glm::vec4(1.0f));

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
			Engine::Get().SubmitToMainThread([this]() {  Engine::Get().Close(); });
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