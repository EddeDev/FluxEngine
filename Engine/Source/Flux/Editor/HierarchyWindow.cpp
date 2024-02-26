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
			Entity entity = m_Scene->CreateEmpty(fmt::format("Entity {0}", entityIndex));

			if (m_SelectedEntity && m_SelectedEntity != m_Scene->GetRootEntity())
				entity.SetParent(m_SelectedEntity);
		}

		DrawEntityNode(m_Scene->GetRootEntity());

		if (ImGui::BeginDragDropTargetCustom({ bounds[0].x, bounds[0].y, bounds[1].x, bounds[1].y }, ImGui::GetCurrentWindow()->ID))
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy_Entity", ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
			{
				const Guid& draggedEntityGUID = *(Guid*)payload->Data;
				Entity draggedEntity = m_Scene->GetEntityFromGUID(draggedEntityGUID);
				draggedEntity.Unparent();
			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		ImGui::Begin("Entity Details");

		if (m_SelectedEntity)
		{
			auto& relationShipComponent = m_SelectedEntity.GetComponent<RelationshipComponent>();
			uint32 childCount = relationShipComponent.GetChildCount();
		
			Guid guid = m_SelectedEntity.GetGUID();
			Guid firstChildGuid = relationShipComponent.GetFirstChild();
			Guid previousGuid = relationShipComponent.GetPrevious();
			Guid nextGuid = relationShipComponent.GetNext();
			Guid parentGuid = relationShipComponent.GetParent();

			std::string guidString = guid.ToString();
			std::string firstChildGuidString = firstChildGuid.ToString();
			std::string previousGuidString = previousGuid.ToString();
			std::string nextGuidString = nextGuid.ToString();
			std::string parentGuidString = parentGuid.ToString();

			std::string firstChildEntityName = firstChildGuid.IsValid() ? m_Scene->GetEntityFromGUID(firstChildGuid).GetComponent<NameComponent>().GetName() : "None";
			std::string previousEntityName = previousGuid.IsValid() ? m_Scene->GetEntityFromGUID(previousGuid).GetComponent<NameComponent>().GetName() : "None";
			std::string nextEntityName = nextGuid.IsValid() ? m_Scene->GetEntityFromGUID(nextGuid).GetComponent<NameComponent>().GetName() : "None";
			std::string parentEntityName = parentGuid.IsValid() ? m_Scene->GetEntityFromGUID(parentGuid).GetComponent<NameComponent>().GetName() : "None";

			ImGui::Text("GUID: %s", guid.IsValid() ? guidString.c_str() : "None");
			ImGui::Separator();
			ImGui::Text("Child Count: %d", childCount);
			ImGui::Text("First Child: %s (%s)", firstChildEntityName.c_str(), firstChildGuid.IsValid() ? firstChildGuidString.c_str() : "None");
			ImGui::Text("Previous: %s (%s)", previousEntityName.c_str(), previousGuid.IsValid() ? previousGuidString.c_str() : "None");
			ImGui::Text("Next: %s (%s)", nextEntityName.c_str(), nextGuid.IsValid() ? nextGuidString.c_str() : "None");
			ImGui::Text("Parent: %s (%s)", parentEntityName.c_str(), parentGuid.IsValid() ? parentGuidString.c_str() : "None");
			ImGui::Text("Is Child Of Scene Entity: %s", m_SelectedEntity.IsChildOf(m_Scene->GetRootEntity()) ? "True" : "False");
		}

		ImGui::End();
	}

	void HierarchyWindow::DrawEntityNode(Entity entity)
	{
		std::string entityIDString = fmt::format("Entity_{0}_{1}", (uint32)entity, (uintptr)entity.GetScene());
		std::string dragHandleIDString = fmt::format("{0}_DragHandle", entityIDString);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 1.5f });

		ImGui::InvisibleButton(dragHandleIDString.c_str(), { ImGui::GetWindowContentRegionMax().x, 2.0f }, ImGuiButtonFlags_None);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = CustomDragDropPayload("Hierarchy_Entity"))
			{
				const Guid& draggedEntityGUID = *(Guid*)payload->Data;
				Entity draggedEntity = m_Scene->GetEntityFromGUID(draggedEntityGUID);
			}
			ImGui::EndDragDropTarget();
		}

		ImGuiTreeNodeFlags flags = 0;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		if (m_SelectedEntity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;
		if (entity == m_Scene->GetRootEntity())
		{
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
			flags |= ImGuiTreeNodeFlags_AllowItemOverlap;
			flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
		}
		else
		{
			if (!entity.HasChildren())
				flags |= ImGuiTreeNodeFlags_Leaf;
		}

		std::string entityName = entity.GetComponent<NameComponent>().GetName();
		bool open = ImGui::TreeNodeEx(entityName.c_str(), flags);

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("Hierarchy_Entity", &entity.GetGUID(), sizeof(Guid));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy_Entity"))
			{
				const Guid& draggedEntityGUID = *(Guid*)payload->Data;
				Entity draggedEntity = m_Scene->GetEntityFromGUID(draggedEntityGUID);
				if (draggedEntity)
					draggedEntity.SetParent(entity);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			m_SelectedEntity = entity;

		ImGui::PopStyleVar();

		if (open)
		{
			for (Entity childEntity : entity.GetChildren())
				DrawEntityNode(childEntity);

			ImGui::TreePop();
		}
	}

}