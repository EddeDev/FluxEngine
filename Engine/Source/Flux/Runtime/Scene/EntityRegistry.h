#pragma once

#include "EntityID.h"

namespace Flux {

	using ComponentPtr = void*;

	class EntityRegistry
	{
	public:
		EntityID Create()
		{
			auto& map = m_Components[++m_EntityIDCounter];
			FLUX_VERIFY(map.empty());
			return m_EntityIDCounter;
		}

		template<typename T, typename... TArgs>
		T& Emplace(EntityID entityID, TArgs&&... args)
		{
			auto it = m_Components.find(entityID);
			FLUX_VERIFY(it != m_Components.end(), "Entity does not exist!");

			size_t hash = typeid(T).hash_code();
			FLUX_VERIFY(it->second.find(hash) == it->second.end(), "Component already exists!");

			T* component = new T(std::forward<TArgs>(args)...);
			it->second[hash] = component;
			return *component;
		}

		template<typename T>
		T& Get(EntityID entityID) const
		{
			auto it = m_Components.find(entityID);
			FLUX_VERIFY(it != m_Components.end(), "Entity does not exist!");

			size_t hash = typeid(T).hash_code();

			auto mapIt = it->second.find(hash);
			FLUX_VERIFY(mapIt != it->second.end(), "Component does not exist!");
			return *(T*)mapIt->second;
		}

		template<typename T>
		T* TryGet(EntityID entityID) const
		{
			auto it = m_Components.find(entityID);
			FLUX_VERIFY(it != m_Components.end(), "Entity does not exist!");

			size_t hash = typeid(T).hash_code();

			auto mapIt = it->second.find(hash);
			if (mapIt != it->second.end())
				return (T*)mapIt->second;

			return nullptr;
		}

		template<typename T>
		bool Remove(EntityID entityID)
		{
			auto it = m_Components.find(entityID);
			FLUX_VERIFY(it != m_Components.end(), "Entity does not exist!");

			size_t hash = typeid(T).hash_code();

			auto mapIt = it->second.find(hash);
			if (mapIt != it->second.end())
			{
				delete mapIt->second;
				it->second.erase(mapIt);
				return true;
			}

			return false;
		}

		template<typename T>
		bool Has(EntityID entityID) const
		{
			auto it = m_Components.find(entityID);
			FLUX_VERIFY(it != m_Components.end(), "Entity does not exist!");

			size_t hash = typeid(T).hash_code();
			return it->second.find(hash) != it->second.end();
		}

		template<typename TComponent>
		std::vector<TComponent*> GetComponents(EntityID entityID) const
		{
			auto it = m_Components.find(entityID);
			FLUX_VERIFY(it != m_Components.end(), "Entity does not exist!");

			std::vector<TComponent*> result;
			for (auto& [hash, component] : it->second)
				result.push_back((TComponent*)component);
			return result;
		}
	private:
		std::unordered_map<EntityID, std::unordered_map<size_t, ComponentPtr>> m_Components;
		EntityID m_EntityIDCounter = 0;
	};

}