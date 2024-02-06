#include "FluxPCH.h"
#include "EditorCamera.h"

namespace Flux {

	EditorCamera::EditorCamera()
	{
	}

	void EditorCamera::OnUpdate(float deltaTime)
	{
		bool isUsing = Input::GetMouseButton(MouseButtonCode::ButtonRight);

		// FLUX_INFO("IsUsing: {0} ({1}, {2})", isUsing, Input::GetAxis("Mouse X"), Input::GetAxis("Mouse Y"));
		if (isUsing)
		{
			float mouseX = Input::GetAxis("Mouse X");
			float mouseY = Input::GetAxis("Mouse Y");

			m_TargetRotation.X += mouseY * 15.0f * deltaTime;
			m_TargetRotation.Y += mouseX * 15.0f * deltaTime;

			m_TargetRotation.X = Math::Clamp(m_TargetRotation.X, -89.0f, 89.0f);
		}

		float x = Input::GetAxis("Horizontal");
		float y = Input::GetAxis("Vertical");

		m_Boost += Input::GetMouseScrollDeltaY() * 0.1f;

		if (isUsing && (x != 0.0f || y != 0.0f))
		{
			Vector3 translation = Vector3(x, 0.0f, y) * deltaTime;
			if (Input::GetKey(KeyCode::LeftShift))
				translation *= 5.0f;
		
			translation *= Math::Pow(2.0f, m_Boost);

			Quaternion orientation(m_Rotation * Math::DegToRad);
			Vector3 rotatedTranslation = orientation * translation;
			m_TargetPosition += rotatedTranslation;
		}

		float positionLerpPct = 1.0f - Math::Exp((Math::Log(1.0f - 0.99f) / 0.2f) * deltaTime);
		float rotationLerpPct = 1.0f - Math::Exp((Math::Log(1.0f - 0.99f) / 0.05f) * deltaTime);
		m_Position = Vector3::Lerp(m_Position, m_TargetPosition, positionLerpPct);
		m_Rotation = Vector3::Lerp(m_Rotation, m_TargetRotation, rotationLerpPct);

		m_ViewMatrix = Matrix4x4::Inverse(Math::BuildTransformationMatrix(m_Position, m_Rotation * Math::DegToRad));
	}

	void EditorCamera::SetViewportSize(uint32 width, uint32 height)
	{
		float aspectRatio = (float)width / (float)height;
		m_ProjectionMatrix = Matrix4x4::Perspective(m_VerticalFOV, aspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::SetPosition(const Vector3& position)
	{
		m_Position = position;
		m_TargetPosition = position;
	}

	void EditorCamera::SetRotation(const Vector3& rotation)
	{
		m_Rotation = rotation;
		m_TargetRotation = rotation;
	}

}