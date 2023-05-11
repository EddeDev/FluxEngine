#include "FluxPCH.h"
#include "EditorEngine.h"

namespace Flux {

	enum MenuItems : uint32
	{
		// File
		Menu_File_NewProject,
		Menu_File_OpenProject,
		Menu_File_SaveProject,
		Menu_File_Restart,
		Menu_File_Exit,

		// About
		Menu_About_AboutFluxEngine
	};

	void EditorEngine::OnInit()
	{
		WindowMenu fileMenu = m_Window->CreateMenu();
		m_Window->AddMenu(fileMenu, Menu_File_NewProject, "New Project...", true);
		m_Window->AddMenu(fileMenu, Menu_File_OpenProject, "Open Project...");
		m_Window->AddMenu(fileMenu, Menu_File_SaveProject, "Save Project", true);
		m_Window->AddMenuSeparator(fileMenu);
		m_Window->AddMenu(fileMenu, Menu_File_Restart, "Restart");
		m_Window->AddMenu(fileMenu, Menu_File_Exit, "Exit\tAlt+F4");
		
		WindowMenu aboutMenu = m_Window->CreateMenu();
		m_Window->AddMenu(aboutMenu, Menu_About_AboutFluxEngine, "About Flux Engine", true);

		WindowMenu menu = m_Window->CreateMenu();
		m_Window->AddPopupMenu(menu, fileMenu, "File");
		m_Window->AddPopupMenu(menu, aboutMenu, "About");

		m_Window->SetMenu(menu);
		m_Window->AddMenuCallback(FLUX_BIND_CALLBACK(OnMenuCallback, this));

		WindowCreateInfo aboutWindowCreateInfo;
		aboutWindowCreateInfo.Width = 640;
		aboutWindowCreateInfo.Height = 410;
		aboutWindowCreateInfo.Title = "About Flux Engine";
		aboutWindowCreateInfo.Resizable = false;
		aboutWindowCreateInfo.ParentWindow = m_Window.get();

		m_AboutWindow = Window::Create(aboutWindowCreateInfo);
	}

	void EditorEngine::OnExit()
	{
		m_AboutWindow.reset();
	}

	void EditorEngine::OnUpdate()
	{
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
			Close(true);
			break;
		}
		case Menu_File_Exit:
		{
			Close();
			break;
		}
		case Menu_About_AboutFluxEngine:
		{
			m_AboutWindow->SetVisible(true);
			break;
		}
		}
	}

}