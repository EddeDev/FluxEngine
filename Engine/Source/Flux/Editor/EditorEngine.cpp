#include "FluxPCH.h"
#include "EditorEngine.h"

namespace Flux {

	void EditorEngine::OnInit()
	{
		Shared<WindowMenu> menu = WindowMenu::Create();

		Shared<WindowMenu> fileMenu = menu->AddMenu("File");
		fileMenu->AddMenu("Open Scene");
		fileMenu->AddSeparator();
		fileMenu->AddMenu("Restart", [this]() { Close(true); });
		fileMenu->AddMenu("Exit", [this]() { Close(); });

		Shared<WindowMenu> helpMenu = menu->AddMenu("Help");
		helpMenu->AddMenu("About");

		m_Window->SetMenu(menu);
	}

	void EditorEngine::OnExit()
	{
	}

	void EditorEngine::OnUpdate()
	{
	}

}