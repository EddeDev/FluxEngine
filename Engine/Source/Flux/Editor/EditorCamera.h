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

		const Vector3& GetPosition() const { return m_Position; }
		const Vector3& GetRotation() const { return m_Rotation; }

		const Matrix4x4& GetViewMatrix() const { return m_ViewMatrix; }
		const Matrix4x4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	private:
		Matrix4x4 m_ViewMatrix = Matrix4x4(1.0f);
		Matrix4x4 m_ProjectionMatrix = Matrix4x4(1.0f);

		Vector3 m_TargetPosition = Vector3(0.0f);
		Vector3 m_Position = Vector3(0.0f);

		Vector3 m_TargetRotation = Vector3(0.0f);
		Vector3 m_Rotation = Vector3(0.0f);

		float m_Boost = 3.5f;

		float m_VerticalFOV = 60.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 100.0f;
	};

}