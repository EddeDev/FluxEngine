#include "FluxPCH.h"
#include "Component.h"

#include "Entity.h"

namespace Flux {

#pragma region Transform
	Transform::Transform(const Vector3& position, const Vector3& eulerAngles, const Vector3& scale)
		: m_Position(position), m_Rotation(eulerAngles), m_Scale(scale)
	{
	}

	void Transform::OnInit()
	{
		RecalculateTransform();
	}

	void Transform::OnImGuiRender()
	{

	}

	void Transform::RecalculateTransform()
	{
		Entity entity = { m_EntityID, m_Scene };

		Matrix4x4 localTransform = Math::BuildTransformationMatrix(m_Position, m_Rotation, m_Scale);
		if (entity.HasParent())
		{
			Entity parent = entity.GetParent();
			m_WorldTransform = parent.GetComponent<Transform>().GetWorldTransform() * localTransform;
		}
		else
		{
			m_WorldTransform = localTransform;
		}

		for (Entity child : entity.GetChildren())
			child.GetComponent<Transform>().RecalculateTransform();
	}

	void Transform::SetPosition(const Vector3& position)
	{
		if (Vector3::EpsilonNotEqual(m_Position, position))
		{
			m_Position = position;
			RecalculateTransform();
		}
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{
		// if (Quaternion::EpsilonNotEqual(m_Rotation, rotation))
		{
			m_Rotation = rotation;
			RecalculateTransform();
		}
	}

	void Transform::SetScale(const Vector3& scale)
	{
		if (Vector3::EpsilonNotEqual(m_Scale, scale))
		{
			m_Scale = scale;
			RecalculateTransform();
		}
	}
#pragma endregion Transform

#pragma region Submesh
	Submesh::Submesh(Ref<Mesh> mesh, uint32 submeshIndex)
		: m_Mesh(mesh), m_SubmeshIndex(submeshIndex)
	{
	}
#pragma endregion Submesh

#pragma region MeshRenderer
	void MeshRenderer::OnRender(Ref<RenderPipeline> pipeline)
	{
		Entity entity = { m_EntityID, m_Scene };

		if (entity.HasComponent<Submesh>())
		{
			auto& submesh = entity.GetComponent<Submesh>();
			auto& transform = entity.GetComponent<Transform>();

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