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
		MeshRenderer,
		Light
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
		virtual void OnViewportResize(uint32 width, uint32 height) {}
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

		void SetLocalPosition(const Vector3& position);
		const Vector3& GetLocalPosition() const { return m_LocalPosition; }

		void SetLocalRotation(const Quaternion& rotation);
		const Quaternion& GetLocalRotation() const { return m_LocalRotation; }

		void SetLocalEulerAngles(const Vector3& eulerAngles);
		const Vector3& GetLocalEulerAngles() const { return m_LocalEulerAngles; }

		void SetScale(const Vector3& scale);
		const Vector3& GetScale() const { return m_LocalScale; }

		void SetWorldPosition(const Vector3& position);
		const Vector3& GetWorldPosition() const { return m_WorldPosition; }

		void SetWorldRotation(const Quaternion& rotation);
		const Quaternion& GetWorldRotation() const { return m_WorldRotation; }

		void SetWorldScale(const Vector3& scale);
		const Vector3& GetWorldScale() const { return m_WorldScale; }

		const Matrix4x4& GetLocalTransform() const { return m_LocalTransform; }
		const Matrix4x4& GetWorldTransform() const { return m_WorldTransform; }

		virtual void OnImGuiRender() override;

		COMPONENT_CLASS_TYPE(Transform)
	private:
		void RecalculateTransform();

		friend class Entity;
	private:
		Vector3 m_LocalPosition;
		Quaternion m_LocalRotation;
		Vector3 m_LocalEulerAngles;
		Vector3 m_LocalScale;

		Vector3 m_WorldPosition;
		Quaternion m_WorldRotation;
		Vector3 m_WorldScale;

		Matrix4x4 m_LocalTransform;
		Matrix4x4 m_WorldTransform;
	};

	class CameraComponent : public Component
	{
	public:
		enum class ProjectionType : uint8
		{
			Perspective = 0,
			Orthographic
		};
	public:
		virtual void OnViewportResize(uint32 width, uint32 height) override;

		const Matrix4x4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

		void SetProjectionType(ProjectionType type);
		ProjectionType GetProjectionType() const { return m_ProjectionType; }

		void SetNearClip(float nearClip);
		float GetNearClip() const { return m_NearClip; }

		void SetFarClip(float farClip);
		float GetFarClip() const { return m_FarClip; }

		void SetFieldOfView(float fieldOfView);
		float GetFieldOfView() const { return m_FieldOfView; }

		COMPONENT_CLASS_TYPE(Camera)
	private:
		void RecalculateProjectionMatrix();
	private:
		Matrix4x4 m_ProjectionMatrix;

		ProjectionType m_ProjectionType = ProjectionType::Perspective;
		float m_NearClip = 0.1f;
		float m_FarClip = 1000.0f;
		float m_FieldOfView = 60.0f;
		float m_AspectRatio = 0.0f;
	};

	class SubmeshComponent : public Component
	{
	public:
		SubmeshComponent(Ref<Mesh> mesh = nullptr, uint32 submeshIndex = 0);

		void SetMesh(Ref<Mesh> mesh);
		Ref<Mesh> GetMesh() const { return m_Mesh; }

		void SetSubmeshIndex(uint32 index);
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

	class LightComponent : public Component
	{
	public:
		enum class LightType : uint8
		{
			Directional = 0
		};
	public:
		void SetLightType(LightType type);
		LightType GetLightType() const { return m_Type; }
		
		void SetColor(const Vector3& color);
		const Vector3& GetColor() const { return m_Color; }

		COMPONENT_CLASS_TYPE(Light)
	private:
		LightType m_Type = LightType::Directional;
		Vector3 m_Color = Vector3(1.0f);
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
		MeshRendererComponent,
		LightComponent
	>;

}