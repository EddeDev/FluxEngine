#include "FluxPCH.h"
#include "Component.h"

#include "Entity.h"

#include "Flux/Runtime/Asset/AssetDatabase.h"

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
		: m_LocalPosition(position), m_LocalEulerAngles(eulerAngles), m_LocalRotation(eulerAngles * Math::DegToRad), m_LocalScale(scale)
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

		m_LocalTransform = Math::BuildTransformationMatrix(m_LocalPosition, m_LocalRotation, m_LocalScale);

		Entity parent = entity.GetParent();
		if (parent && parent.HasComponent<TransformComponent>())
			m_WorldTransform = parent.GetComponent<TransformComponent>().GetWorldTransform() * m_LocalTransform;
		else
			m_WorldTransform = m_LocalTransform;

		Math::DecomposeTransformationMatrix(m_WorldTransform, m_WorldPosition, m_WorldRotation, m_WorldScale);

		for (Entity child : entity.GetChildren())
		{
			if (child.HasComponent<TransformComponent>())
				child.GetComponent<TransformComponent>().RecalculateTransform();
		}

		OnChanged();
	}

	void TransformComponent::SetLocalPosition(const Vector3& position)
	{
		if (Vector3::EpsilonNotEqual(m_LocalPosition, position))
		{
			m_LocalPosition = position;
			RecalculateTransform();
		}
	}

	void TransformComponent::SetLocalRotation(const Quaternion& rotation)
	{
		if (Quaternion::EpsilonNotEqual(m_LocalRotation, rotation))
		{
			Vector3 previousEulerAngles = m_LocalEulerAngles * Math::DegToRad;
			m_LocalRotation = rotation;
			Vector3 newEulerAngles = m_LocalRotation.GetEulerAngles();
			m_LocalEulerAngles = newEulerAngles * Math::RadToDeg;

			if ((Math::Abs(newEulerAngles.X - previousEulerAngles.X) == Math::PI) &&
				(Math::Abs(newEulerAngles.Z - previousEulerAngles.Z) == Math::PI))
			{
				m_LocalEulerAngles = Vector3(previousEulerAngles.X, Math::PI - newEulerAngles.Y, previousEulerAngles.Z) * Math::RadToDeg;
			}

			RecalculateTransform();
		}
	}

	void TransformComponent::SetLocalEulerAngles(const Vector3& eulerAngles)
	{
		if (Vector3::EpsilonNotEqual(m_LocalEulerAngles, eulerAngles))
		{
			m_LocalEulerAngles = eulerAngles;
			m_LocalRotation = Quaternion(eulerAngles * Math::DegToRad);
			RecalculateTransform();
		}
	}

	void TransformComponent::SetScale(const Vector3& scale)
	{
		if (Vector3::EpsilonNotEqual(m_LocalScale, scale))
		{
			m_LocalScale = scale;
			RecalculateTransform();
		}
	}

	void TransformComponent::SetWorldPosition(const Vector3& position)
	{
		SetLocalPosition(m_LocalPosition + position - m_WorldPosition);
	}

	void TransformComponent::SetWorldRotation(const Quaternion& rotation)
	{
		Quaternion rotationOffset = m_WorldRotation * Quaternion::Inverse(m_LocalRotation);
		Quaternion worldRotation = rotation * rotationOffset;
		SetLocalRotation(Quaternion::Inverse(rotationOffset) * worldRotation);
	}

	void TransformComponent::SetWorldScale(const Vector3& scale)
	{
		SetScale(m_LocalScale + scale - m_WorldScale);
	}
#pragma endregion Transform

#pragma region Camera
	void CameraComponent::OnViewportResize(uint32 width, uint32 height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjectionMatrix();
	}

	void CameraComponent::RecalculateProjectionMatrix()
	{
		FLUX_VERIFY(m_AspectRatio > 0.0f);

		switch (m_ProjectionType)
		{
		case CameraComponent::ProjectionType::Perspective:
		{
			m_ProjectionMatrix = Matrix4x4::Perspective(m_FieldOfView, m_AspectRatio, m_FarClip, m_NearClip);
			break;
		}
		case CameraComponent::ProjectionType::Orthographic:
		{
			FLUX_VERIFY(false, "Not implemented!");
			break;
		}
		}

		OnChanged();
	}

	void CameraComponent::SetProjectionType(ProjectionType type)
	{
		if (m_ProjectionType != type)
		{
			m_ProjectionType = type;
			RecalculateProjectionMatrix();
		}
	}

	void CameraComponent::SetNearClip(float nearClip)
	{
		if (m_NearClip != nearClip)
		{
			m_NearClip = nearClip;
			RecalculateProjectionMatrix();
		}
	}

	void CameraComponent::SetFarClip(float farClip)
	{
		if (m_FarClip != farClip)
		{
			m_FarClip = farClip;
			RecalculateProjectionMatrix();
		}
	}

	void CameraComponent::SetFieldOfView(float fieldOfView)
	{
		if (m_FieldOfView != fieldOfView)
		{
			m_FieldOfView = fieldOfView;
			RecalculateProjectionMatrix();
		}
	}
#pragma endregion Camera

#pragma region Submesh
	SubmeshComponent::SubmeshComponent(const AssetID& meshAssetID, uint32 submeshIndex)
		: m_MeshAssetID(meshAssetID), m_SubmeshIndex(submeshIndex)
	{
	}

	void SubmeshComponent::SetMeshAssetID(const AssetID& assetID)
	{
		if (m_MeshAssetID != assetID)
		{
			m_MeshAssetID = assetID;
			OnChanged();
		}
	}

	void SubmeshComponent::SetSubmeshIndex(uint32 index)
	{
		if (m_SubmeshIndex != index)
		{
			m_SubmeshIndex = index;
			OnChanged();
		}
	}
#pragma endregion Submesh

#pragma region MeshRenderer
	void MeshRendererComponent::OnRender(Ref<RenderPipeline> pipeline)
	{
		Entity entity = { m_Entity, m_Scene };

		if (entity.HasComponent<SubmeshComponent>())
		{
			auto& submeshComponent = entity.GetComponent<SubmeshComponent>();
			auto& transformComponent = entity.GetComponent<TransformComponent>();

			auto& meshAssetID = submeshComponent.GetMeshAssetID();
			Ref<Mesh> mesh = AssetDatabase::GetAssetFromID<Mesh>(meshAssetID);
			if (mesh)
			{
				DynamicMeshSubmitInfo submitInfo;
				submitInfo.Mesh = mesh;
				submitInfo.SubmeshIndex = submeshComponent.GetSubmeshIndex();
				submitInfo.Transform = transformComponent.GetWorldTransform();

				pipeline->SubmitDynamicMesh(submitInfo);
			}
		}
	}
#pragma endregion MeshRenderer

#pragma region Light
	void LightComponent::SetLightType(LightType type)
	{
		if (m_Type != type)
		{
			m_Type = type;
			OnChanged();
		}
	}

	void LightComponent::SetColor(const Vector3& color)
	{
		if (Vector3::EpsilonNotEqual(m_Color, color))
		{
			m_Color = color;
			OnChanged();
		}
	}
#pragma endregion Light

}