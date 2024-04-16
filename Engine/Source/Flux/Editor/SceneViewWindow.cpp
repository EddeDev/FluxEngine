#include "FluxPCH.h"
#include "SceneViewWindow.h"

// TODO: temp
#include "HierarchyWindow.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/ImGui/ImGuiUtils.h"
#include "Flux/Runtime/ImGui/ImGuizmo.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Flux {

	SceneViewWindow::SceneViewWindow()
	{
		m_Padding = { 0.0f, 0.0f };

		m_NoDecoration = true;
		m_NoScrollbar = true;
		m_NoScrollWithMouse = true;

		m_RenderPipeline = Ref<ForwardRenderPipeline>::Create();
	}

	SceneViewWindow::~SceneViewWindow()
	{
	}

	void SceneViewWindow::OnUpdate()
	{
		if (m_Scene && m_ViewportWidth > 0 && m_ViewportHeight > 0)
		{
			float deltaTime = Engine::Get().GetDeltaTime();

			m_EditorCamera.SetActive(m_IsViewportFocused && m_IsViewportHovered);
			m_EditorCamera.OnUpdate(deltaTime);

			m_EditorCamera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
			m_RenderPipeline->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
			m_Scene->SetViewportSize(m_ViewportWidth, m_ViewportHeight);

			m_Scene->OnRender(m_RenderPipeline, m_EditorCamera.GetViewMatrix(), m_EditorCamera.GetProjectionMatrix());
		}
	}

	void SceneViewWindow::OnImGuiRender()
	{
		if (m_Scene)
		{
			ImVec2 minRegion = ImGui::GetWindowContentRegionMin();
			ImVec2 maxRegion = ImGui::GetWindowContentRegionMax();
			ImVec2 viewportSize = { maxRegion.x - minRegion.x, maxRegion.y - minRegion.y };

			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 bounds[2];
			bounds[0] = { minRegion.x + windowPos.x, minRegion.y + windowPos.y };
			bounds[1] = { maxRegion.x + windowPos.x, maxRegion.y + windowPos.y };

			m_ViewportWidth = (uint32)viewportSize.x;
			m_ViewportHeight = (uint32)viewportSize.y;

			m_IsViewportHovered = ImGui::IsWindowHovered();
			m_IsViewportFocused = ImGui::IsWindowFocused();

			if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			{
				UI::Image(m_RenderPipeline->GetComposedTexture(), viewportSize, { 0.0f, 1.0f }, { 1.0f, 0.0f });

				DrawGizmos();
			}
		}
	}

	void SceneViewWindow::DrawGizmos()
	{
		if (m_GizmoType == GizmoType::None)
			return;

		Ref<HierarchyWindow> hierarchyWindow = EditorWindowManager::GetWindow<HierarchyWindow>();
		if (!hierarchyWindow)
			return;

		Entity selectedEntity = hierarchyWindow->GetSelectedEntity();
		if (!selectedEntity)
			return;

		if (!selectedEntity.HasComponent<TransformComponent>())
			return;

		auto& transformComponent = selectedEntity.GetComponent<TransformComponent>();

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

		ImGuizmo::OPERATION operation = (ImGuizmo::OPERATION)0;
		switch (m_GizmoType)
		{
		case GizmoType::Translate: operation = ImGuizmo::TRANSLATE; break;
		case GizmoType::Rotate: operation = ImGuizmo::ROTATE; break;
		case GizmoType::Scale: operation = ImGuizmo::SCALE; break;
		}

		bool snap = Input::GetKey(KeyCode::LeftControl);
		float snapValue = 0.5f;
		if (m_GizmoType == GizmoType::Rotate)
			snapValue = 45.0f;
		float snapValues[3] = { snapValue, snapValue, snapValue };

		Matrix4x4 worldTransform = transformComponent.GetWorldTransform();
		ImGuizmo::Manipulate(m_EditorCamera.GetViewMatrix().GetPointer(), m_EditorCamera.GetProjectionMatrix().GetPointer(), 
			operation, ImGuizmo::WORLD, worldTransform.GetPointer(),
			nullptr, snap ? snapValues : nullptr);

		if (ImGuizmo::IsUsing())
		{
			Vector3 worldPosition;
			Quaternion worldRotation;
			Vector3 worldScale;
			Math::DecomposeTransformationMatrix(worldTransform, worldPosition, worldRotation, worldScale);

			transformComponent.SetWorldPosition(worldPosition);
			transformComponent.SetWorldRotation(worldRotation);
			transformComponent.SetWorldScale(worldScale);
		}
	}

	void SceneViewWindow::OnEvent(Event& event)
	{
		EventHandler handler(event);
		handler.Bind<KeyPressedEvent>(FLUX_BIND_CALLBACK(OnKeyPressedEvent, this));
		handler.Bind<MouseButtonPressedEvent>(FLUX_BIND_CALLBACK(OnMouseButtonPressedEvent, this));
	}

	void SceneViewWindow::OnKeyPressedEvent(KeyPressedEvent& event)
	{
		if (m_EditorCamera.IsUsing())
			return;

		if (ImGui::GetActiveID())
			return;

		switch (event.GetKey())
		{
		case KeyCode::Q: m_GizmoType = GizmoType::None; break;
		case KeyCode::W: m_GizmoType = GizmoType::Translate; break;
		case KeyCode::E: m_GizmoType = GizmoType::Rotate; break;
		case KeyCode::R: m_GizmoType = GizmoType::Scale; break;
		}
	}

	void SceneViewWindow::OnMouseButtonPressedEvent(MouseButtonPressedEvent& event)
	{
		switch (event.GetButton())
		{
		case MouseButtonCode::ButtonLeft:
		{
			if (m_IsViewportHovered && !ImGuizmo::IsOver())
			{
				// TODO: mouse picking
				// set selected entity
			}
			break;
		}
		}
	}

}