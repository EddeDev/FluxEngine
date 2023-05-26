#include "FluxPCH.h"
#include "EditorEngine.h"

#include "EditorUI.h"

namespace Flux {

	void EditorEngine::OnInit()
	{
		SubmitToEventThread([this]()
		{
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

			WindowCreateInfo windowCreateInfo;
			windowCreateInfo.ParentWindow = m_Window.get();

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
		});

		UI::Init();
	}

	void EditorEngine::OnExit()
	{
		UI::Shutdown();

		SubmitToEventThread([this]()
		{
			m_Windows.clear();
		});
	}

	void EditorEngine::OnUpdate()
	{
		UI::BeginFrame();
		UI::EndFrame();
	}

	void EditorEngine::OnMenuCallback(WindowMenu menu, uint32 itemID)
	{
		switch (itemID)
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
			SubmitToMainThread([this]() { Close(true); });
			break;
		}
		case Menu_File_Exit:
		{
			SubmitToMainThread([this]() { Close(); });
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