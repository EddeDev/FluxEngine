#include "FluxPCH.h"
#include "Scene.h"

#include "Entity.h"

namespace Flux {

	Scene::Scene()
	{
		// TODO: scene asset id
		Guid rootEntityGUID = Guid::NewGuid();
		m_SceneEntity = new Entity(m_Registry.create(), this);
		m_EntityMap[rootEntityGUID] = *m_SceneEntity;

		m_SceneEntity->AddComponent<IDComponent>().SetGUID(rootEntityGUID);
		m_SceneEntity->AddComponent<NameComponent>().SetName("Scene");
		m_SceneEntity->AddComponent<RelationshipComponent>();

		RegisterComponentCallbacks(AllComponents{}, m_Registry);
	}

	Scene::~Scene()
	{
		// TODO: properly destroy scene entity

		delete m_SceneEntity;
		m_SceneEntity = nullptr;
	}

	void Scene::OnUpdate()
	{
		// Update all entities
	}

	void Scene::OnRender(Ref<RenderPipeline> pipeline, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix)
	{
		auto& cameraSettings = pipeline->GetCameraSettings();
		cameraSettings.ViewMatrix = viewMatrix;
		cameraSettings.ProjectionMatrix = projectionMatrix;
		cameraSettings.NearClip = 0.1f;
		cameraSettings.FarClip = 1000.0f;

		pipeline->BeginRendering();
		// Render all entities
		pipeline->EndRendering();
	}

	Entity Scene::CreateEmpty(const std::string& name)
	{
		return CreateEmpty(name, Guid::NewGuid());
	}

	Entity Scene::CreateEmpty(const std::string& name, const Guid& guid)
	{
		Entity entity{ m_Registry.create(), this };

		m_EntityMap[guid] = entity;

		entity.AddComponent<IDComponent>().SetGUID(guid);
		entity.AddComponent<NameComponent>().SetName(name);
		entity.AddComponent<RelationshipComponent>();
		entity.AddComponent<TransformComponent>();

		entity.SetParent(*m_SceneEntity);
		return entity;
	}

	Entity Scene::GetEntityFromGUID(const Guid& guid)
	{
		auto it = m_EntityMap.find(guid);
		if (it != m_EntityMap.end())
			return it->second;
		return {};
	}

	bool Scene::RemoveChild(Entity child)
	{
		if (!child || !child.HasParent())
			return false;

		return true;
	}

	void Scene::OnComponentAdded(Entity entity, Component& component)
	{
		component.SetEntity(entity, this);
		component.OnInit();
	}

}