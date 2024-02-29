#pragma once

#include "EditorWindow.h"

#include "EditorCamera.h"

#include "Flux/Runtime/Renderer/RenderPipeline.h"

namespace Flux {

	class SceneViewWindow : public EditorWindow
	{
	public:
		SceneViewWindow();
		virtual ~SceneViewWindow();

		virtual void OnUpdate() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	private:
		void DrawGizmos();

		void OnKeyPressedEvent(KeyPressedEvent& event);
		void OnMouseButtonPressedEvent(MouseButtonPressedEvent& event);
	private:
		Ref<RenderPipeline> m_RenderPipeline;

		EditorCamera m_EditorCamera;

		uint32 m_ViewportWidth = 0;
		uint32 m_ViewportHeight = 0;

		bool m_IsViewportHovered = false;

		enum class GizmoType
		{
			None = 0,

			Translate,
			Rotate,
			Scale
		} m_GizmoType = GizmoType::None;
	};

}