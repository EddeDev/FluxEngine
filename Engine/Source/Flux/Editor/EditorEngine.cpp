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
		m_Window->AddMenu(fileMenu, Menu_File_NewProject, "New Project...");
		m_Window->AddMenu(fileMenu, Menu_File_OpenProject, "Open Project...");
		m_Window->AddMenu(fileMenu, Menu_File_SaveProject, "Save Project");
		m_Window->AddMenuSeparator(fileMenu);
		m_Window->AddMenu(fileMenu, Menu_File_Restart, "Restart");
		m_Window->AddMenu(fileMenu, Menu_File_Exit, "Exit\tAlt+F4");
		
		WindowMenu aboutMenu = m_Window->CreateMenu();
		m_Window->AddMenu(aboutMenu, Menu_About_AboutFluxEngine, "About Flux Engine");

		WindowMenu menu = m_Window->CreateMenu();
		m_Window->AddPopupMenu(menu, fileMenu, "File");
		m_Window->AddPopupMenu(menu, aboutMenu, "About");

		m_Window->SetMenu(menu);
		m_Window->AddMenuCallback(FLUX_BIND_CALLBACK(OnMenuCallback, this));
	}

	void EditorEngine::OnExit()
	{
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
			DialogResult result = Platform::OpenFolderDialog(m_Window.get(), &outPath, "Load Project");
			if (result == DialogResult::Ok)
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
			break;
		}
		}
	}

}