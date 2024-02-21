#pragma once

#include "EntityID.h"

namespace Flux {

	enum class ComponentType
	{
		None = 0,

		Transform
	};

#define COMPONENT_CLASS_TYPE(type) \
	static ComponentType GetStaticType() { return ComponentType::type; } \
	ComponentType GetType() const { return GetStaticType(); }

	class Component;

	using ComponentChangedCallback = std::function<void(Component*)>;

	class Component
	{
	public:
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
		void SetEntityID(EntityID entityID) { m_EntityID = entityID; }

		friend class Entity;
	private:
		EntityID m_EntityID;
		ComponentChangedCallback m_Callback;
	};

	class Transform : public Component
	{
	public:
		Transform(const Vector3& position = Vector3(0.0f), const Vector3& eulerAngles = Vector3(0.0f), const Vector3& scale = Vector3(1.0f));
	
		virtual void OnImGuiRender() override;

		COMPONENT_CLASS_TYPE(Transform)
	private:
		Vector3 m_Position;
		Quaternion m_Rotation;
		Vector3 m_Scale;
	};

}