#include "FluxPCH.h"
#include "Component.h"

#include "Entity.h"

namespace Flux {

#pragma region Name
	void NameComponent::SetName(const std::string& name)
	{
		if (m_Name != name)
		{
			m_Name = name;
			OnChanged();
		}
	}
#pragma endregion Name

#pragma region Transform
	TransformComponent::TransformComponent(const Vector3& position, const Vector3& eulerAngles, const Vector3& scale)
		: m_Position(position), m_Rotation(eulerAngles), m_Scale(scale)
	{
	}

	void TransformComponent::OnInit()
	{
		RecalculateTransform();
	}

	void TransformComponent::OnImGuiRender()
	{

	}

	void TransformComponent::RecalculateTransform()
	{
		Entity entity = { m_Entity, m_Scene };

		Matrix4x4 localTransform = Math::BuildTransformationMatrix(m_Position, m_Rotation, m_Scale);

		Entity parent = entity.GetParent();
		if (parent && parent.HasComponent<TransformComponent>())
			m_WorldTransform = parent.GetComponent<TransformComponent>().GetWorldTransform() * localTransform;
		else
			m_WorldTransform = localTransform;

		for (Entity child : entity.GetChildren())
		{
			if (child.HasComponent<TransformComponent>())
				child.GetComponent<TransformComponent>().RecalculateTransform();
		}
	}

	void TransformComponent::SetPosition(const Vector3& position)
	{
		if (Vector3::EpsilonNotEqual(m_Position, position))
		{
			m_Position = position;
			RecalculateTransform();
		}
	}

	void TransformComponent::SetRotation(const Quaternion& rotation)
	{
		// if (Quaternion::EpsilonNotEqual(m_Rotation, rotation))
		{
			m_Rotation = rotation;
			RecalculateTransform();
		}
	}

	void TransformComponent::SetScale(const Vector3& scale)
	{
		if (Vector3::EpsilonNotEqual(m_Scale, scale))
		{
			m_Scale = scale;
			RecalculateTransform();
		}
	}
#pragma endregion Transform

#pragma region Submesh
	SubmeshComponent::SubmeshComponent(Ref<Mesh> mesh, uint32 submeshIndex)
		: m_Mesh(mesh), m_SubmeshIndex(submeshIndex)
	{
	}
#pragma endregion Submesh

#pragma region MeshRenderer
	void MeshRendererComponent::OnRender(Ref<RenderPipeline> pipeline)
	{
		Entity entity = { m_Entity, m_Scene };

		if (entity.HasComponent<SubmeshComponent>())
		{
			auto& submesh = entity.GetComponent<SubmeshComponent>();
			auto& transform = entity.GetComponent<TransformComponent>();

			Ref<Mesh> mesh = submesh.GetMesh();
			if (mesh)
			{
				DynamicMeshSubmitInfo submitInfo;
				submitInfo.Mesh = mesh;
				submitInfo.SubmeshIndex = submesh.GetSubmeshIndex();
				submitInfo.Transform = transform.GetWorldTransform();

				pipeline->SubmitDynamicMesh(submitInfo);
			}
		}
	}
#pragma endregion MeshRenderer

}