#pragma once

#include "EntityRegistry.h"
#include "Component.h"

namespace Flux {

	struct EntityData
	{
		std::string Name;

		EntityID Parent = NullEntity;
		std::vector<EntityID> Children;
	};

	class Entity;

	class Scene
	{
	public:
		Entity CreateEntity(const std::string& name);

		std::vector<Entity> GetEntities();
		std::vector<Entity> GetRootEntities();

		EntityData& GetEntityData(EntityID entityID) { return m_EntityData.at(entityID); }
		const EntityData& GetEntityData(EntityID entityID) const { return m_EntityData.at(entityID); }

		EntityRegistry& GetRegistry() { return m_Registry; }
		const EntityRegistry& GetRegistry() const { return m_Registry; }
	private:
		EntityRegistry m_Registry;
		std::unordered_map<EntityID, EntityData> m_EntityData;
	};

}