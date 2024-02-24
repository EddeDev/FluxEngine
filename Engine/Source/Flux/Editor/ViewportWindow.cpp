#include "FluxPCH.h"
#include "ViewportWindow.h"

#include "Flux/Runtime/Core/Engine.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Flux {

	ViewportWindow::ViewportWindow()
	{
		m_Padding = { 0.0f, 0.0f };

		m_NoDecoration = true;
		m_NoScrollbar = true;
		m_NoScrollWithMouse = true;

		m_RenderPipeline = Ref<ForwardRenderPipeline>::Create();

		m_SphereMesh = Mesh::LoadFromFile("Resources/Meshes/Sphere.glb");
	}

	ViewportWindow::~ViewportWindow()
	{
	}

	void ViewportWindow::OnUpdate()
	{
		if (!m_Scene || m_ViewportWidth == 0 || m_ViewportHeight == 0)
			return;

		// TODO: remove
		if (m_Scene->GetEntities().empty())
		{
			Entity sphereEntity = m_Scene->CreateEntity("Sphere");
			sphereEntity.AddComponent<Submesh>(m_SphereMesh);
			sphereEntity.AddComponent<MeshRenderer>();
		}

		Entity sphereEntity = m_Scene->GetEntity(0);

		auto& sphereTransform = sphereEntity.GetComponent<Transform>();
		sphereTransform.SetPosition(Vector3(Math::Cos(Engine::Get().GetTime()), Math::Sin(Engine::Get().GetTime()), 0.0f));

		float deltaTime = Engine::Get().GetDeltaTime();
		m_EditorCamera.OnUpdate(deltaTime);

		m_EditorCamera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		m_RenderPipeline->SetViewportSize(m_ViewportWidth, m_ViewportHeight);

		m_Scene->OnRender(m_RenderPipeline, m_EditorCamera.GetViewMatrix(), m_EditorCamera.GetProjectionMatrix());
	}

	void ViewportWindow::OnImGuiRender()
	{
		if (!m_Scene)
			return;

		ImVec2 minRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 maxRegion = ImGui::GetWindowContentRegionMax();

		ImVec2 viewportSize = { maxRegion.x - minRegion.x, maxRegion.y - minRegion.y };

		m_ViewportWidth = (uint32)viewportSize.x;
		m_ViewportHeight = (uint32)viewportSize.y;

		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
		{
			Ref<ImGuiRenderer> imGuiRenderer = Engine::Get().GetImGuiRenderer();
			imGuiRenderer->Image(m_RenderPipeline->GetComposedTexture(), viewportSize, { 0.0f, 1.0f }, { 1.0f, 0.0f });
		}
	}

}