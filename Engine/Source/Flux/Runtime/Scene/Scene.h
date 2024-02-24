#pragma once

#include "EntityRegistry.h"
#include "Component.h"

#include "Flux/Runtime/Renderer/RenderPipeline.h"

namespace Flux {

	struct EntityData
	{
		std::string Name;

		EntityID Parent = NullEntity;
		std::vector<EntityID> Children;
	};

	class Entity;

	class Scene : public ReferenceCounted
	{
	public:
		Scene() {}
		virtual ~Scene() {}

		void OnUpdate();
		void OnRender(Ref<RenderPipeline> pipeline, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix);

		Entity CreateEntity(const std::string& name);

		Entity GetEntity(uint32 index);
		void MoveEntity(Entity entity, uint32 newIndex);
		uint32 GetEntityIndex(Entity entity) const;

		Entity GetPreviousEntity(Entity entity);
		Entity GetNextEntity(Entity entity);

		std::vector<Entity> GetEntities();
		std::vector<Entity> GetRootEntities();

		EntityData& GetEntityData(EntityID entityID) { return m_EntityData.at(entityID); }
		const EntityData& GetEntityData(EntityID entityID) const { return m_EntityData.at(entityID); }

		std::vector<EntityID>& GetEntityIDs() { return m_EntityIDs; }
		const std::vector<EntityID>& GetEntityIDs() const { return m_EntityIDs; }

		EntityRegistry& GetRegistry() { return m_Registry; }
		const EntityRegistry& GetRegistry() const { return m_Registry; }
	private:
		EntityRegistry m_Registry;
		std::unordered_map<EntityID, EntityData> m_EntityData;
		std::vector<EntityID> m_EntityIDs;
	};

}