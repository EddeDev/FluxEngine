#include "FluxPCH.h"
#include "GameViewWindow.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/ImGui/ImGuiUtils.h"

namespace Flux {

	GameViewWindow::GameViewWindow()
	{
		m_Padding = { 0.0f, 0.0f };

		m_NoDecoration = true;
		m_NoScrollbar = true;
		m_NoScrollWithMouse = true;

		m_RenderPipeline = Ref<ForwardRenderPipeline>::Create();
	}

	GameViewWindow::~GameViewWindow()
	{
	}

	void GameViewWindow::OnUpdate()
	{
		if (m_Scene && m_ViewportWidth > 0 && m_ViewportHeight > 0)
		{
			m_RenderPipeline->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
			m_Scene->SetViewportSize(m_ViewportWidth, m_ViewportHeight);

			m_Scene->OnRender(m_RenderPipeline);
		}
	}

	void GameViewWindow::OnImGuiRender()
	{
		if (m_Scene)
		{
			ImVec2 minRegion = ImGui::GetWindowContentRegionMin();
			ImVec2 maxRegion = ImGui::GetWindowContentRegionMax();

			ImVec2 viewportSize = { maxRegion.x - minRegion.x, maxRegion.y - minRegion.y };

			m_ViewportWidth = (uint32)viewportSize.x;
			m_ViewportHeight = (uint32)viewportSize.y;

			m_IsViewportHovered = ImGui::IsWindowHovered();

			if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
				UI::Image(m_RenderPipeline->GetComposedTexture(), viewportSize, { 0.0f, 1.0f }, { 1.0f, 0.0f });
		}
	}

}