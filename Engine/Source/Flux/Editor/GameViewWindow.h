#pragma once

#include "EditorWindow.h"

#include "Flux/Runtime/Renderer/RenderPipeline.h"

namespace Flux {

	class GameViewWindow : public EditorWindow
	{
	public:
		GameViewWindow();
		virtual ~GameViewWindow();

		virtual void OnUpdate() override;
		virtual void OnImGuiRender() override;
	private:
		Ref<RenderPipeline> m_RenderPipeline;

		uint32 m_ViewportWidth = 0;
		uint32 m_ViewportHeight = 0;

		bool m_IsViewportHovered = false;
	};

}