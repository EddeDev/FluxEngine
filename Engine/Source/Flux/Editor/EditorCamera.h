#pragma once

namespace Flux {

	class EditorCamera
	{
	public:
		EditorCamera();

		void OnUpdate(float deltaTime);
		void SetViewportSize(uint32 width, uint32 height);

		void SetPosition(const Vector3& position);
		void SetRotation(const Vector3& rotation);

		void SetActive(bool active) { m_IsActive = active; }
		bool IsActive() const { return m_IsActive; }

		bool IsUsing() const { return m_IsUsing; }

		const Vector3& GetPosition() const { return m_Position; }
		const Vector3& GetRotation() const { return m_Rotation; }

		const Matrix4x4& GetViewMatrix() const { return m_ViewMatrix; }
		const Matrix4x4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const Matrix4x4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
		const Matrix4x4& GetInverseViewProjectionMatrix() const { return m_InverseViewProjectionMatrix; }

		float GetNearClip() const { return m_NearClip; }
		float GetFarClip() const { return m_FarClip; }
	private:
		void RecalculateViewMatrix();
		void RecalculateViewProjectionMatrix();
	private:
		Matrix4x4 m_ViewMatrix = Matrix4x4(1.0f);
		Matrix4x4 m_ProjectionMatrix = Matrix4x4(1.0f);
		Matrix4x4 m_ViewProjectionMatrix = Matrix4x4(1.0f);
		Matrix4x4 m_InverseViewProjectionMatrix = Matrix4x4(1.0f);

		Vector3 m_TargetPosition = Vector3(0.0f);
		Vector3 m_Position = Vector3(0.0f);
		float m_PositionLerpTime = 0.2f;

		Vector3 m_TargetRotation = Vector3(0.0f);
		Vector3 m_Rotation = Vector3(0.0f);
		float m_RotationLerpTime = 0.01f;

		float m_Boost = 3.5f;

		bool m_IsActive = false;
		bool m_IsUsing = false;

		float m_VerticalFOV = 60.0f;
		float m_NearClip = 0.01f;
		float m_FarClip = 1000.0f;
		float m_AspectRatio = 1.0f;
	};

}