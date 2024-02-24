#include "FluxPCH.h"
#include "Scene.h"

#include "Entity.h"

namespace Flux {

	void Scene::OnUpdate()
	{
		for (EntityID entityID : m_EntityIDs)
		{
			for (Component* component : m_Registry.GetComponents<Component>(entityID))
				component->OnUpdate();
		}
	}

	void Scene::OnRender(Ref<RenderPipeline> pipeline, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix)
	{
		auto& cameraSettings = pipeline->GetCameraSettings();
		cameraSettings.ViewMatrix = viewMatrix;
		cameraSettings.ProjectionMatrix = projectionMatrix;
		cameraSettings.NearClip = 0.1f;
		cameraSettings.FarClip = 1000.0f;

		pipeline->BeginRendering();

		for (EntityID entityID : m_EntityIDs)
		{
			for (Component* component : m_Registry.GetComponents<Component>(entityID))
				component->OnRender(pipeline);
		}

		pipeline->EndRendering();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity{ m_Registry.Create(), this };

		auto& entityData = m_EntityData[entity];
		entityData.Name = name;
		m_EntityIDs.push_back(entity);

		entity.AddComponent<Transform>();
		return entity;
	}

	Entity Scene::GetEntity(uint32 index)
	{
		if (index < m_EntityIDs.size())
			return { m_EntityIDs[index], this };
		return {};
	}

	void Scene::MoveEntity(Entity entity, uint32 newIndex)
	{
		auto it = std::find(m_EntityIDs.begin(), m_EntityIDs.end(), entity);
		if (it != m_EntityIDs.end())
		{
			if (it - m_EntityIDs.begin() == newIndex)
				return;

			m_EntityIDs.erase(it);
			m_EntityIDs.insert(m_EntityIDs.begin() + newIndex, entity);
		}
	}

	uint32 Scene::GetEntityIndex(Entity entity) const
	{
		auto it = std::find(m_EntityIDs.begin(), m_EntityIDs.end(), entity);
		if (it != m_EntityIDs.end())
			return it - m_EntityIDs.begin();

		FLUX_VERIFY(false);
		return 0;
	}

	Entity Scene::GetPreviousEntity(Entity entity)
	{
		uint32 entityIndex = GetEntityIndex(entity);
		if (entityIndex > 0)
			return { m_EntityIDs[entityIndex - 1], this };
		return {};
	}

	Entity Scene::GetNextEntity(Entity entity)
	{
		uint32 entityIndex = GetEntityIndex(entity);
		if (entityIndex < m_EntityIDs.size() - 1)
			return { m_EntityIDs[entityIndex + 1], this };
		return {};
	}

	std::vector<Entity> Scene::GetEntities()
	{
		std::vector<Entity> result(m_EntityIDs.size());
		for (size_t i = 0; i < m_EntityIDs.size(); i++)
			result[i] = { m_EntityIDs[i], this };
		return result;
	}

	std::vector<Entity> Scene::GetRootEntities()
	{
		std::vector<Entity> result;
		for (size_t i = 0; i < m_EntityIDs.size(); i++)
		{
			EntityID entityID = m_EntityIDs[i];
			if (m_EntityData[entityID].Parent == NullEntity)
				result.push_back({ m_EntityIDs[i], this });
		}
		return result;
	}

}