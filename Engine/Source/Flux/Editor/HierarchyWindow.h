#pragma once

#include "EditorWindow.h"

namespace Flux {

	class HierarchyWindow : public EditorWindow
	{
	public:
		HierarchyWindow();
		virtual ~HierarchyWindow();

		virtual void OnImGuiRender() override;

		Entity GetSelectedEntity() const { return m_SelectedEntity; }
	private:
		void DrawEntityNode(Entity entity);
	private:
		Entity m_SelectedEntity;
	};

}