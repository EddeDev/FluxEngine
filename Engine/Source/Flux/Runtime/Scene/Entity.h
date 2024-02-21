#pragma once

#include "Scene.h"

namespace Flux {

	class Entity
	{
	public:
		Entity();
		Entity(EntityID entityID, Scene* scene);
	
		template<typename T, typename... TArgs>
		T& AddComponent(TArgs&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value);

			T& component = m_Scene->GetRegistry().Emplace<T>(m_EntityID, std::forward<TArgs>(args)...);
			component.SetEntityID(m_EntityID);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			static_assert(std::is_base_of<Component, T>::value);

			return m_Scene->GetRegistry().Get<T>(m_EntityID);
		}

		template<typename T>
		T* TryGetComponent()
		{
			static_assert(std::is_base_of<Component, T>::value);

			return m_Scene->GetRegistry().TryGet<T>(m_EntityID);
		}

		template<typename T>
		bool RemoveComponent()
		{
			static_assert(std::is_base_of<Component, T>::value);

			return m_Scene->GetRegistry().Remove<T>(m_EntityID);
		}

		template<typename T>
		bool HasComponent() const
		{
			static_assert(std::is_base_of<Component, T>::value);

			return m_Scene->GetRegistry().Has<T>(m_EntityID);
		}

		std::vector<Component*> GetComponents() const;

		bool operator==(Entity other) const
		{
			if (!other)
				return false;

			return m_EntityID == other.m_EntityID && m_Scene == other.m_Scene;
		}

		bool operator==(EntityID other) const
		{
			if (other == NullEntity)
				return false;

			return m_EntityID == other;
		}

		operator EntityID() const { return m_EntityID; }
		operator bool() const { return m_EntityID != NullEntity && m_Scene != nullptr; }

		void SetParent(Entity parent);

		void AddChild(Entity child);
		bool RemoveChild(Entity child);

		void Unparent();
		void SetParentID(EntityID entityID);

		bool IsParentOf(Entity entity) const;
		bool IsChildOf(Entity entity) const;

		void SetName(const std::string& name) const;
		const std::string& GetName() const;

		Entity GetParent() const;
		bool HasParent() const;
		bool HasChildren() const;
		std::vector<Entity> GetChildren() const;
	private:
		EntityID m_EntityID;
		Scene* m_Scene;
	};

}