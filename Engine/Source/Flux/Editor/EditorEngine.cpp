#include "FluxPCH.h"
#include "EditorEngine.h"

namespace Flux {

	enum MenuCommands : uint32
	{
		MENU_Test,
		MENU_Exit
	};

	void EditorEngine::OnInit()
	{
		WindowMenu fileMenu = Platform::CreateMenu();
		Platform::AddMenu(fileMenu, MENU_Test, "Test :)");
		Platform::AddMenuSeparator(fileMenu);
		Platform::AddMenu(fileMenu, MENU_Exit, "Exit\tAlt+F4");

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

	void EditorEngine::OnMenuCallback(WindowMenu menu, uint32 menuID)
	{
		switch (menuID)
		{
		case MENU_Test:
		{
			__debugbreak();
			break;
		}
		case MENU_Exit:
		{
			Close();
			break;
		}
		}
	}

}