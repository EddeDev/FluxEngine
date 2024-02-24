#pragma once

#include "EditorWindow.h"

#include "EditorCamera.h"

#include "Flux/Runtime/Renderer/RenderPipeline.h"

namespace Flux {

	class ViewportWindow : public EditorWindow
	{
	public:
		ViewportWindow();
		virtual ~ViewportWindow();

		virtual void OnUpdate() override;
		virtual void OnImGuiRender() override;
	private:
		Ref<Mesh> m_SphereMesh;
		Ref<RenderPipeline> m_RenderPipeline;

		EditorCamera m_EditorCamera;

		uint32 m_ViewportWidth = 0;
		uint32 m_ViewportHeight = 0;
	};

}