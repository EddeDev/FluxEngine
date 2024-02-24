#include "FluxPCH.h"
#include "InspectorWindow.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Flux {

	InspectorWindow::InspectorWindow()
	{
	}

	InspectorWindow::~InspectorWindow()
	{
	}

	void InspectorWindow::OnImGuiRender()
	{
		if (!m_Scene)
			return;

		ImVec2 minRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 maxRegion = ImGui::GetWindowContentRegionMax();
		ImVec2 windowPos = ImGui::GetWindowPos();

		ImVec2 bounds[2];
		bounds[0] = { minRegion.x + windowPos.x, minRegion.y + windowPos.y };
		bounds[1] = { maxRegion.x + windowPos.x, maxRegion.y + windowPos.y };
	}

}