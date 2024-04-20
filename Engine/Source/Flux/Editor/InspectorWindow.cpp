#include "FluxPCH.h"
#include "InspectorWindow.h"

// TODO: temp
#include "HierarchyWindow.h"

#include "Flux/Runtime/ImGui/ImGuiUtils.h"
#include "Flux/Runtime/Asset/AssetDatabase.h"

#include <imgui_internal.h>

namespace Flux {

	InspectorWindow::InspectorWindow()
	{
	}

	InspectorWindow::~InspectorWindow()
	{
	}

	template<typename TComponent, bool IsRemovable = true, typename TFunction>
	static void DrawComponent(const std::string& name, Entity entity, TFunction function)
	{
		if (entity.HasComponent<TComponent>())
		{
			auto& component = entity.GetComponent<TComponent>();
			ImGui::PushID(&component);

			float contentRegionAvailX = ImGui::GetContentRegionAvail().x;

			ImGuiTreeNodeFlags flags = 0;
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
			flags |= ImGuiTreeNodeFlags_Framed;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			flags |= ImGuiTreeNodeFlags_AllowItemOverlap;
			flags |= ImGuiTreeNodeFlags_FramePadding;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.5f, 2.5f });
			float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
			// ImGui::Separator();
			bool open = ImGui::TreeNodeEx("##DummyID", flags, name.c_str());
			bool rightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionAvailX - lineHeight * 0.5f);
			if (ImGui::Button("+", { lineHeight, lineHeight }) || rightClicked)
				ImGui::OpenPopup("ComponentSettings");

			bool removeComponent = false;
			bool resetComponent = false;

			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Reset"))
					resetComponent = true;

				if constexpr (IsRemovable)
				{
					if (ImGui::MenuItem("Remove"))
						removeComponent = true;
				}

				ImGui::EndPopup();
			}

			if (open)
			{
				function(component);
				ImGui::TreePop();
			}

			// TODO: Reset function?
			if (removeComponent || resetComponent)
				entity.RemoveComponent<TComponent>();
			if (resetComponent)
				entity.AddComponent<TComponent>();

			ImGui::PopID();
		}
	}

	void InspectorWindow::OnImGuiRender()
	{
		if (!m_Scene)
			return;

		Ref<HierarchyWindow> hierarchyWindow = EditorWindowManager::GetWindow<HierarchyWindow>();
		if (!hierarchyWindow)
			return;

		Entity selectedEntity = hierarchyWindow->GetSelectedEntity();
		if (!selectedEntity)
			return;

		DrawComponent<TransformComponent, false>("Transform", selectedEntity, [&](TransformComponent& component)
		{
			UI::BeginPropertyGrid();

			Vector3 position = component.GetLocalPosition();
			if (DrawVector3Control("Position", position))
				component.SetLocalPosition(position);

			Vector3 eulerAngles = component.GetLocalEulerAngles();
			if (DrawVector3Control("Rotation", eulerAngles))
				component.SetLocalEulerAngles(eulerAngles);

			Vector3 scale = component.GetLocalScale();
			if (DrawVector3Control("Scale", scale, 1.0f))
				component.SetLocalScale(scale);

			ImGui::Separator();

#if 0
			ImGui::BeginDisabled();
		
			const Matrix4x4& worldTransform = component.GetWorldTransform();

			Vector3 worldPosition;
			Quaternion worldRotation;
			Vector3 worldScale;
			Math::DecomposeTransformationMatrix(worldTransform, worldPosition, worldRotation, worldScale);

			Vector3 worldEulerAngles = worldRotation.GetEulerAngles() * Math::RadToDeg;

			DrawVector3Control("World Position", worldPosition);
			DrawVector3Control("World Rotation", worldEulerAngles);
			DrawVector3Control("World Scale", worldScale);

			ImGui::EndDisabled();
#endif

			UI::EndPropertyGrid();
		});

		DrawComponent<CameraComponent>("Camera", selectedEntity, [&](CameraComponent& component)
		{
			UI::BeginPropertyGrid();

			float fieldOfView = component.GetFieldOfView();
			if (UI::Property("Field of View", fieldOfView, 0.1f, 0.0f, 179.0f))
				component.SetFieldOfView(fieldOfView);

			float nearClip = component.GetNearClip();
			if (UI::Property("Near Clip", nearClip, 0.001f, 0.01f, std::numeric_limits<float>::max()))
				component.SetNearClip(nearClip);

			float farClip = component.GetFarClip();
			if (UI::Property("Far Clip", farClip, 1.0f, 0.0f, std::numeric_limits<float>::max()))
				component.SetFarClip(farClip);

			UI::EndPropertyGrid();
		});

		DrawComponent<SubmeshComponent>("Submesh", selectedEntity, [&](SubmeshComponent& component)
		{
			UI::BeginPropertyGrid();

			Ref<Mesh> mesh = AssetDatabase::GetAssetFromID<Mesh>(component.GetMeshAssetID());
			if (mesh)
			{
				int32 submeshIndex = component.GetSubmeshIndex();
			
				int32 maxSubmeshIndex = (int32)mesh->GetProperties().Submeshes.size();
				if (maxSubmeshIndex > 0)
					maxSubmeshIndex--;
				UI::Property("Submesh Index", submeshIndex, 0.1f, 0, maxSubmeshIndex);
			}

			UI::EndPropertyGrid();
		});

		DrawComponent<MeshRendererComponent>("Mesh Renderer", selectedEntity, [&](MeshRendererComponent& component)
		{
			UI::BeginPropertyGrid();

			UI::EndPropertyGrid();
		});

		DrawComponent<LightComponent>("Light", selectedEntity, [&](LightComponent& component)
		{
			UI::BeginPropertyGrid();
			
			UI::EndPropertyGrid();
		});
	}

	bool InspectorWindow::DrawVector3Control(const std::string& label, Vector3& values, float resetValue, float columnWidth)
	{
		bool modified = false;

#if 0
		if (Math::EpsilonEqual(values.X, 0.0f))
			values.X = 0.0f;
		if (Math::EpsilonEqual(values.Y, 0.0f))
			values.Y = 0.0f;
		if (Math::EpsilonEqual(values.Z, 0.0f))
			values.Z = 0.0f;
#endif

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.05f);

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 buttonSize(lineHeight + 3.0f, lineHeight);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
		{
			values.X = resetValue;
			modified = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##X", &values.X, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });

		if (ImGui::Button("Y", buttonSize))
		{
			values.Y = resetValue;
			modified = true;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##Y", &values.Y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });

		if (ImGui::Button("Z", buttonSize))
		{
			values.Z = resetValue;
			modified = true;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##Z", &values.Z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar(2);
		ImGui::Columns(1);
		ImGui::PopID();

		return modified;
	}

	bool InspectorWindow::DrawVector4Control(const std::string& label, Vector4& values, float resetValue, float columnWidth)
	{
		bool modified = false;

#if 0
		if (Math::EpsilonEqual(values.X, 0.0f))
			values.X = 0.0f;
		if (Math::EpsilonEqual(values.Y, 0.0f))
			values.Y = 0.0f;
		if (Math::EpsilonEqual(values.Z, 0.0f))
			values.Z = 0.0f;
		if (Math::EpsilonEqual(values.W, 0.0f))
			values.W = 0.0f;
#endif

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.05f);

		float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
		ImVec2 buttonSize(lineHeight + 3.0f, lineHeight);

		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
			if (ImGui::Button("X", buttonSize))
			{
				values.X = resetValue;
				modified = true;
			}
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			modified |= ImGui::DragFloat("##X", &values.X, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
			if (ImGui::Button("Y", buttonSize))
			{
				values.Y = resetValue;
				modified = true;
			}
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			modified |= ImGui::DragFloat("##Y", &values.Y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}
	
		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });

			if (ImGui::Button("Z", buttonSize))
			{
				values.Z = resetValue;
				modified = true;
			}

			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			modified |= ImGui::DragFloat("##Z", &values.Z, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		{
#if 0
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });
#endif

			if (ImGui::Button("W", buttonSize))
			{
				values.W = resetValue;
				modified = true;
			}

#if 0
			ImGui::PopStyleColor(3);
#endif

			ImGui::SameLine();
			modified |= ImGui::DragFloat("##W", &values.W, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
		}
		
		ImGui::PopStyleVar(2);
		ImGui::Columns(1);
		ImGui::PopID();

		return modified;
	}

}