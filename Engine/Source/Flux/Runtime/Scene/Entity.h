#pragma once

#include "Scene.h"

namespace Flux {

	class Entity
	{
	public:
		Entity();
		Entity(entt::entity entity, Scene* scene);
	
		template<typename T, typename... TArgs>
		T& AddComponent(TArgs&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value);

			return m_Scene->GetRegistry().emplace<T>(m_Entity, std::forward<TArgs>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			static_assert(std::is_base_of<Component, T>::value);

			return m_Scene->GetRegistry().get<T>(m_Entity);
		}

		template<typename T>
		const T& GetComponent() const
		{
			static_assert(std::is_base_of<Component, T>::value);

			return m_Scene->GetRegistry().get<T>(m_Entity);
		}

		template<typename T>
		T* TryGetComponent()
		{
			static_assert(std::is_base_of<Component, T>::value);

			return m_Scene->GetRegistry().try_get<T>(m_Entity);
		}

		template<typename T>
		bool RemoveComponent()
		{
			static_assert(std::is_base_of<Component, T>::value);

			return m_Scene->GetRegistry().remove<T>(m_Entity);
		}

		template<typename T>
		bool HasComponent() const
		{
			static_assert(std::is_base_of<Component, T>::value);

			return m_Scene->GetRegistry().any_of<T>(m_Entity);
		}

		operator bool() const { return m_Entity != entt::null && m_Scene != nullptr; }
		operator entt::entity() const { return m_Entity; }
		operator uint32() const { return (uint32)m_Entity; }

		bool operator==(Entity other) const { return m_Entity == other.m_Entity && m_Scene == other.m_Scene; }
		bool operator!=(Entity other) const { return !(*this == other); }

		const Guid& GetGUID() const;
		const Guid& GetParentGUID() const;
		void SetParentGUID(const Guid& guid);
		bool HasParent() const;
		bool HasChildren() const;
		bool IsParentOf(Entity entity);
		bool IsChildOf(Entity entity);
		Entity GetParent() const;

		void SetParent(Entity parent);

		std::vector<Guid> GetChildrenGUIDs() const;
		std::vector<Entity> GetChildren();

		Scene* GetScene() const { return m_Scene; }
	private:
		entt::entity m_Entity;
		Scene* m_Scene;
	};

}