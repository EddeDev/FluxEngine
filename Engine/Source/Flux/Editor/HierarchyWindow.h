#pragma once

#include "EditorWindow.h"

namespace Flux {

	class SelectionManager
	{
	public:
		static void SetSelectedEntity(Entity entity)
		{
			s_SelectedEntity = entity;
		}

		static Entity GetSelectedEntity()
		{
			return s_SelectedEntity;
		}
	private:
		inline static Entity s_SelectedEntity;
	};

	class HierarchyWindow : public EditorWindow
	{
	public:
		HierarchyWindow();
		virtual ~HierarchyWindow();

		virtual void OnImGuiRender() override;
	private:
		void DrawEntityNode(Entity entity);
	};

}