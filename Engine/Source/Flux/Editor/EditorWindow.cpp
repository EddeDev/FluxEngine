#include "FluxPCH.h"
#include "EditorWindow.h"

#include <imgui.h>

namespace Flux {

	void EditorWindowManager::Init()
	{
		FLUX_INFO_CATEGORY("Editor Window Manager", "Initializing...");
		FLUX_VERIFY(s_Windows.empty() && !s_ActiveScene);
	}

	void EditorWindowManager::Shutdown()
	{
		FLUX_INFO_CATEGORY("Editor Window Manager", "Shutting down...");
		s_Windows.clear();
		s_ActiveScene = nullptr;
	}

	void EditorWindowManager::OnUpdate()
	{
		for (auto& [hashCode, data] : s_Windows)
		{
			if (!data.IsOpen)
				continue;
				
			data.Window->OnUpdate();
		}
	}

	void EditorWindowManager::OnImGuiRender()
	{
		for (auto& [hashCode, data] : s_Windows)
		{
			if (!data.IsOpen)
				continue;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { data.Window->m_Padding.X, data.Window->m_Padding.Y });
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { data.Window->m_ItemSpacing.X, data.Window->m_ItemSpacing.Y });

			ImGuiWindowFlags flags = 0;
			if (data.Window->m_NoDecoration)
				flags |= ImGuiWindowFlags_NoDecoration;
			if (data.Window->m_NoScrollbar)
				flags |= ImGuiWindowFlags_NoScrollbar;
			if (data.Window->m_NoScrollWithMouse)
				flags |= ImGuiWindowFlags_NoScrollWithMouse;

			data.Window->SetScene(s_ActiveScene);

			ImGui::Begin(data.Title.data(), &data.IsOpen, flags);
			ImGui::PushID(data.HashCode);
			data.Window->OnImGuiRender();
			ImGui::PopID();
			ImGui::End();

			ImGui::PopStyleVar(2);
		}
	}

}