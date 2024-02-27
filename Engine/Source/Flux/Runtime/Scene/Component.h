#pragma once

#include "Flux/Runtime/Renderer/RenderPipeline.h"

#include <entt/entt.hpp>

namespace Flux {

	enum class ComponentType : uint8
	{
		None = 0,

		Name,
		ID,
		Relationship,
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

		virtual ComponentType GetType() const = 0;
	private:
		void SetEntity(entt::entity entity, Scene* scene)
		{
			m_Entity = entity;
			m_Scene = scene;
		}

		friend class Scene;
	protected:
		void OnChanged()
		{
			if (m_Callback)
				m_Callback(this);
		}
	protected:
		entt::entity m_Entity;
		Scene* m_Scene;

		ComponentChangedCallback m_Callback;
	};

	class IDComponent : public Component
	{
	public:
		void SetGUID(const Guid& guid) { m_GUID = guid; }
		const Guid& GetGUID() const { return m_GUID; }

		COMPONENT_CLASS_TYPE(ID)
	private:
		Guid m_GUID;
	};

	class NameComponent : public Component
	{
	public:
		void SetName(const std::string& name);
		const std::string& GetName() const { return m_Name; }

		COMPONENT_CLASS_TYPE(Name)
	private:
		std::string m_Name;
	};

	class RelationshipComponent : public Component
	{
	public:
		void SetChildCount(uint32 childCount) { m_ChildCount = childCount; }
		void IncrementChildCount() { m_ChildCount++; }
		void DecrementChildCount() { FLUX_ASSERT(m_ChildCount > 0); m_ChildCount--; }
		uint32 GetChildCount() const { return m_ChildCount; }

		void SetFirstChild(const Guid& guid) { m_FirstChild = guid; }
		const Guid& GetFirstChild() const { return m_FirstChild; }

		void SetPrevious(const Guid& guid) { m_Previous = guid; }
		const Guid& GetPrevious() const { return m_Previous; }

		void SetNext(const Guid& guid) { m_Next = guid; }
		const Guid& GetNext() const { return m_Next; }

		void SetParent(const Guid& guid) { m_Parent = guid; }
		const Guid& GetParent() const { return m_Parent; }

		COMPONENT_CLASS_TYPE(Relationship)
	private:
		uint32 m_ChildCount = 0;
		Guid m_FirstChild;
		Guid m_Previous;
		Guid m_Next;
		Guid m_Parent;
	};

	class TransformComponent : public Component
	{
	public:
		TransformComponent(const Vector3& position = Vector3(0.0f), const Vector3& eulerAngles = Vector3(0.0f), const Vector3& scale = Vector3(1.0f));
	
		virtual void OnInit();

		void SetPosition(const Vector3& position);
		const Vector3& GetPosition() const { return m_Position; }

		void SetRotation(const Quaternion& rotation);
		const Quaternion& GetRotation() const { return m_Rotation; }

		void SetEulerAngles(const Vector3& eulerAngles);
		const Vector3& GetEulerAngles() const { return m_EulerAngles; }

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
		Vector3 m_EulerAngles;
		Vector3 m_Scale;

		Matrix4x4 m_WorldTransform;
	};

	class CameraComponent : public Component
	{
	public:
		COMPONENT_CLASS_TYPE(Camera)
	};

	class SubmeshComponent : public Component
	{
	public:
		SubmeshComponent(Ref<Mesh> mesh = nullptr, uint32 submeshIndex = 0);

		void SetMesh(Ref<Mesh> mesh) { m_Mesh = mesh; }
		Ref<Mesh> GetMesh() const { return m_Mesh; }

		void SetSubmeshIndex(uint32 index) { m_SubmeshIndex = index; }
		uint32 GetSubmeshIndex() const { return m_SubmeshIndex; }

		COMPONENT_CLASS_TYPE(Submesh)
	private:
		Ref<Mesh> m_Mesh;
		uint32 m_SubmeshIndex;
	};

	class MeshRendererComponent : public Component
	{
	public:
		virtual void OnRender(Ref<RenderPipeline> pipeline) override;

		COMPONENT_CLASS_TYPE(MeshRenderer)
	};

	template<typename... Component>
	class ComponentSet {};

	using AllComponents = ComponentSet<
		IDComponent,
		NameComponent,
		RelationshipComponent,
		TransformComponent,
		CameraComponent,
		SubmeshComponent,
		MeshRendererComponent
	>;


}