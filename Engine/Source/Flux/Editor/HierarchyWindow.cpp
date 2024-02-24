#include "FluxPCH.h"
#include "HierarchyWindow.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Flux {

	static void RenderCustomDragDropTargetRect(const ImRect& bb)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImRect bb_display = bb;
		bb_display.ClipWith(window->ClipRect); // Clip THEN expand so we have a way to visualize that target is not entirely visible.
		bool push_clip_rect = !window->ClipRect.Contains(bb_display);
		if (push_clip_rect)
			window->DrawList->PushClipRectFullScreen();
		window->DrawList->AddRectFilled(bb_display.Min, bb_display.Max, ImGui::GetColorU32(ImGuiCol_DragDropTarget));
		if (push_clip_rect)
			window->DrawList->PopClipRect();
	}

	static const ImGuiPayload* CustomDragDropPayload(const char* type, ImGuiDragDropFlags flags = ImGuiDragDropFlags_None)
	{
		ImGuiContext& g = *GImGui;
		ImGuiPayload& payload = g.DragDropPayload;
		IM_ASSERT(g.DragDropActive);
		IM_ASSERT(payload.DataFrameCount != -1);
		if (type != NULL && !payload.IsDataType(type))
			return NULL;

		const bool was_accepted_previously = (g.DragDropAcceptIdPrev == g.DragDropTargetId);
		ImRect r = g.DragDropTargetRect;
		float r_surface = r.GetWidth() * r.GetHeight();
		if (r_surface > g.DragDropAcceptIdCurrRectSurface)
			return NULL;

		g.DragDropAcceptFlags = flags;
		g.DragDropAcceptIdCurr = g.DragDropTargetId;
		g.DragDropAcceptIdCurrRectSurface = r_surface;

		payload.Preview = was_accepted_previously;
		flags |= (g.DragDropSourceFlags & ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
		if (!(flags & ImGuiDragDropFlags_AcceptNoDrawDefaultRect) && payload.Preview)
			RenderCustomDragDropTargetRect(r);

		g.DragDropAcceptFrameCount = g.FrameCount;
		payload.Delivery = was_accepted_previously && !ImGui::IsMouseDown(g.DragDropMouseButton);
		if (!payload.Delivery && !(flags & ImGuiDragDropFlags_AcceptBeforeDelivery))
			return NULL;

		return &payload;
	}

	HierarchyWindow::HierarchyWindow()
	{
	}

	HierarchyWindow::~HierarchyWindow()
	{
	}

	void HierarchyWindow::OnImGuiRender()
	{
		if (!m_Scene)
			return;

		ImVec2 minRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 maxRegion = ImGui::GetWindowContentRegionMax();
		ImVec2 windowPos = ImGui::GetWindowPos();

		ImVec2 bounds[2];
		bounds[0] = { minRegion.x + windowPos.x, minRegion.y + windowPos.y };
		bounds[1] = { maxRegion.x + windowPos.x, maxRegion.y + windowPos.y };

		if (ImGui::Button("+"))
		{
			static uint32 entityIndex = 0;
			entityIndex++;
			m_Scene->CreateEntity(fmt::format("Entity {0}", entityIndex));
		}

		for (Entity entity : m_Scene->GetRootEntities())
			DrawEntityNode(entity);

		if (ImGui::BeginDragDropTargetCustom({ bounds[0].x, bounds[0].y, bounds[1].x, bounds[1].y }, ImGui::GetCurrentWindow()->ID))
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy_Entity", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
			{
				EntityID draggedEntityID = *(EntityID*)payload->Data;
				Entity draggedEntity(draggedEntityID, m_Scene.Get());
				draggedEntity.Unparent();
			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			SelectionManager::SetSelectedEntity({});
	}

	void HierarchyWindow::DrawEntityNode(Entity entity)
	{
		std::string entityIDString = fmt::format("Entity_{0}_{1}", (uint32)entity, (uintptr)entity.GetScene());
		std::string separatorIDString = fmt::format("{0}_Separator", entityIDString);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 1.5f });

		ImGui::InvisibleButton(separatorIDString.c_str(), { ImGui::GetWindowContentRegionMax().x, 2.0f }, ImGuiButtonFlags_None);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = CustomDragDropPayload("Hierarchy_Entity"))
			{
				EntityID draggedEntityID = *(EntityID*)payload->Data;
				Entity draggedEntity(draggedEntityID, m_Scene.Get());

				uint32 draggedEntityIndex = m_Scene->GetEntityIndex(draggedEntity);
				uint32 entityIndex = m_Scene->GetEntityIndex(entity);

#if 0
				if (draggedEntityIndex > entityIndex)
					m_Scene->MoveEntity(draggedEntity, entityIndex);
				else
					m_Scene->MoveEntity(draggedEntity, entityIndex - 1);
#endif

				FLUX_INFO("dragged: {0} to entity: ~{1}", draggedEntityIndex, entityIndex);
			}
			ImGui::EndDragDropTarget();
		}

		ImGuiTreeNodeFlags flags = 0;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		if (SelectionManager::GetSelectedEntity() == entity)
			flags |= ImGuiTreeNodeFlags_Selected;
		if (!entity.HasChildren())
			flags |= ImGuiTreeNodeFlags_Leaf;

		std::string debugLabel = fmt::format("{0} (index: {1})", entity.GetName(), m_Scene->GetEntityIndex(entity));

#if 1
		bool open = ImGui::TreeNodeEx(debugLabel.c_str(), flags);
#else
		bool open = ImGui::TreeNodeEx(entity.GetName().c_str(), flags);
#endif

		if (ImGui::BeginDragDropSource())
		{
			EntityID entityID = entity;
			ImGui::SetDragDropPayload("Hierarchy_Entity", &entityID, sizeof(EntityID));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy_Entity"))
			{
				EntityID draggedEntityID = *(EntityID*)payload->Data;

				Entity draggedEntity(draggedEntityID, m_Scene.Get());
				draggedEntity.SetParent(entity);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			SelectionManager::SetSelectedEntity(entity);

		ImGui::PopStyleVar();

		if (open)
		{
			for (Entity childEntity : entity.GetChildren())
				DrawEntityNode(childEntity);

			ImGui::TreePop();
		}
	}

}