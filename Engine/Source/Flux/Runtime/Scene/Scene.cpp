#include "FluxPCH.h"
#include "Scene.h"

#include "Entity.h"

namespace Flux {

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity{ m_Registry.Create(), this };
		entity.AddComponent<Transform>();

		auto& entityData = m_EntityData[entity];
		entityData.Name = name;

		return entity;
	}

	std::vector<Entity> Scene::GetEntities()
	{
		std::vector<Entity> result;
		for (auto& [entityID, entityData] : m_EntityData)
			result.push_back({ entityID, this });
		return result;
	}

	std::vector<Entity> Scene::GetRootEntities()
	{
		std::vector<Entity> result;
		for (auto& [entityID, entityData] : m_EntityData)
		{
			if (entityData.Parent == NullEntity)
				result.push_back({ entityID, this });
		}
		return result;
	}

}