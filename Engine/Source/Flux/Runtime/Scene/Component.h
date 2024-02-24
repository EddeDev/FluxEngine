#pragma once

#include "EntityID.h"

#include "Flux/Runtime/Renderer/RenderPipeline.h"

namespace Flux {

	enum class ComponentType
	{
		None = 0,

		Transform,
		Camera,
		Submesh,
		MeshRenderer
	};

#define COMPONENT_CLASS_TYPE(type) \
	static ComponentType GetStaticType() { return ComponentType::type; } \
	ComponentType GetType() const { return GetStaticType(); }

	class Component;
	using ComponentChangedCallback = std::function<void(Component*)>;

	class Scene;

	class Component
	{
	public:
		virtual void OnInit() {}
		virtual void OnUpdate() {}
		virtual void OnRender(Ref<RenderPipeline> pipeline) {}
		virtual void OnImGuiRender() {}
		virtual void SetChangedCallback(const ComponentChangedCallback& callback) { m_Callback = callback; }

		static ComponentType GetStaticType() { return ComponentType::None; }
		virtual ComponentType GetType() const { return GetStaticType(); }
	protected:
		void OnChanged()
		{
			if (m_Callback)
				m_Callback(this);
		}
	private:
		void Init(EntityID entityID, Scene* scene)
		{
			m_EntityID = entityID;
			m_Scene = scene;
		}

		friend class Entity;
	protected:
		EntityID m_EntityID;
		Scene* m_Scene;

		ComponentChangedCallback m_Callback;
	};

	class Transform : public Component
	{
	public:
		Transform(const Vector3& position = Vector3(0.0f), const Vector3& eulerAngles = Vector3(0.0f), const Vector3& scale = Vector3(1.0f));
	
		virtual void OnInit();

		void SetPosition(const Vector3& position);
		const Vector3& GetPosition() const { return m_Position; }

		void SetRotation(const Quaternion& rotation);
		const Quaternion& GetRotation() const { return m_Rotation; }

		void SetScale(const Vector3& scale);
		const Vector3& GetScale() const { return m_Scale; }

		const Matrix4x4& GetWorldTransform() const { return m_WorldTransform; }

		virtual void OnImGuiRender() override;

		COMPONENT_CLASS_TYPE(Transform)
	private:
		void RecalculateTransform();
	private:
		Vector3 m_Position;
		Quaternion m_Rotation;
		Vector3 m_Scale;

		Matrix4x4 m_WorldTransform;
	};

	class Camera : public Component
	{
	public:
		COMPONENT_CLASS_TYPE(Camera)
	};

	class Submesh : public Component
	{
	public:
		Submesh(Ref<Mesh> mesh = nullptr, uint32 submeshIndex = 0);

		void SetMesh(Ref<Mesh> mesh) { m_Mesh = mesh; }
		Ref<Mesh> GetMesh() const { return m_Mesh; }

		void SetSubmeshIndex(uint32 index) { m_SubmeshIndex = index; }
		uint32 GetSubmeshIndex() const { return m_SubmeshIndex; }

		COMPONENT_CLASS_TYPE(Submesh)
	private:
		Ref<Mesh> m_Mesh;
		uint32 m_SubmeshIndex;
	};

	class MeshRenderer : public Component
	{
	public:
		virtual void OnRender(Ref<RenderPipeline> pipeline) override;

		COMPONENT_CLASS_TYPE(MeshRenderer)
	};

}