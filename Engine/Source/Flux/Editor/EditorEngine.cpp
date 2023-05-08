#include "FluxPCH.h"
#include "EditorEngine.h"

namespace Flux {

	void EditorEngine::OnInit()
	{
	

		WindowMenu fileMenu = Platform::CreateMenu();
		Platform::AddMenu(fileMenu, 1 << 0, "Open Scene...");
		Platform::AddMenuSeparator(fileMenu);
		Platform::AddMenu(fileMenu, 1 << 1, "Exit\tAlt+F4");

		WindowMenu menu = Platform::CreateMenu();
		Platform::AddPopupMenu(menu, fileMenu, "File");

		Platform::SetMenu(m_Window.get(), menu);

	}

	void EditorEngine::OnExit()
	{
	}

	void EditorEngine::OnUpdate()
	{
	}

}