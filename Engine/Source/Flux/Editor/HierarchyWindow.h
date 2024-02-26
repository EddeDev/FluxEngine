#pragma once

#include "EditorWindow.h"

namespace Flux {

	class HierarchyWindow : public EditorWindow
	{
	public:
		HierarchyWindow();
		virtual ~HierarchyWindow();

		virtual void OnImGuiRender() override;
	private:
		void DrawEntityNode(Entity entity);
	private:
		Entity m_SelectedEntity;
	};

}