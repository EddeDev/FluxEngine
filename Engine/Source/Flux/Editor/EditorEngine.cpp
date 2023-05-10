#include "FluxPCH.h"
#include "EditorEngine.h"

namespace Flux {

	enum MenuItems : uint32
	{
		Menu_NewProject,
		Menu_OpenProject,
		Menu_SaveProject,
		Menu_Exit
	};

	void EditorEngine::OnInit()
	{
		WindowMenu fileMenu = Platform::CreateMenu();
		Platform::AddMenu(fileMenu, Menu_NewProject, "New Project...");
		Platform::AddMenu(fileMenu, Menu_OpenProject, "Open Project...");
		Platform::AddMenu(fileMenu, Menu_SaveProject, "Save Project");
		Platform::AddMenuSeparator(fileMenu);
		Platform::AddMenu(fileMenu, Menu_Exit, "Exit\tAlt+F4");

		WindowMenu menu = Platform::CreateMenu();
		Platform::AddPopupMenu(menu, fileMenu, "File");

		Platform::SetMenu(m_Window.get(), menu, FLUX_BIND_CALLBACK(OnMenuCallback, this));
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
		case Menu_OpenProject:
		{
			char* outPath = nullptr;
			DialogResult result = Platform::OpenFolderDialog(m_Window.get(), &outPath, "Load Project");
			if (result == DialogResult::Ok)
			{
				FLUX_ERROR("Path: {0}", outPath);
				free(outPath);
			}
			break;
		}
		case Menu_Exit:
		{
			Close();
			break;
		}
		}
	}

}