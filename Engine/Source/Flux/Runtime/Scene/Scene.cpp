#include "FluxPCH.h"
#include "Scene.h"

#include "Entity.h"

#include "Flux/Runtime/Core/Engine.h"

namespace Flux {

	Scene::Scene()
	{
		m_ViewportWidth = Engine::Get().GetMainWindow()->GetWidth();
		m_ViewportHeight = Engine::Get().GetMainWindow()->GetHeight();

		CreateSceneEntity();
		RegisterComponentCallbacks(m_Registry);
	}

	Scene::~Scene()
	{
		DestroySceneEntity();
	}

	void Scene::OnUpdate()
	{
		for (auto entity : m_Registry.view<entt::entity>())
			OnUpdate(entity);
	}

	void Scene::OnRender(Ref<RenderPipeline> pipeline)
	{
		Entity mainCameraEntity = GetMainCameraEntity();
		if (!mainCameraEntity)
			return;

		auto& transformComponent = mainCameraEntity.GetComponent<TransformComponent>();
		auto& cameraComponent = mainCameraEntity.GetComponent<CameraComponent>();

		SceneCameraData cameraData;
		cameraData.ViewMatrix = cameraComponent.GetViewMatrix();
		cameraData.ProjectionMatrix = cameraComponent.GetProjectionMatrix();
		cameraData.ViewProjectionMatrix = cameraComponent.GetViewProjectionMatrix();
		cameraData.InverseViewProjectionMatrix = cameraComponent.GetInverseViewProjectionMatrix();
		cameraData.Position = transformComponent.GetWorldPosition();
		cameraData.NearClip = cameraComponent.GetNearClip();
		cameraData.FarClip = cameraComponent.GetFarClip();

		OnRender(pipeline, cameraData);
	}

	void Scene::OnRender(Ref<RenderPipeline> pipeline, const SceneCameraData& cameraData)
	{
		auto& cameraSettings = pipeline->GetCameraSettings();
		cameraSettings.ViewMatrix = cameraData.ViewMatrix;
		cameraSettings.ProjectionMatrix = cameraData.ProjectionMatrix;
		cameraSettings.ViewProjectionMatrix = cameraData.ViewProjectionMatrix;
		cameraSettings.InverseViewProjectionMatrix = cameraData.InverseViewProjectionMatrix;
		cameraSettings.CameraPosition = cameraData.Position;
		cameraSettings.NearClip = cameraData.NearClip;
		cameraSettings.FarClip = cameraData.FarClip;

		auto& environmentSettings = pipeline->GetEnvironmentSettings();

		auto view = m_Registry.view<const TransformComponent, const LightComponent>();
		for (auto [entity, transformComponent, lightComponent] : view.each())
		{
			auto lightType = lightComponent.GetLightType();
			if (lightType == LightComponent::LightType::Directional)
			{
				environmentSettings.LightDirection = transformComponent.GetWorldRotation() * Vector3(0.0f, 0.0f, 1.0f);
				environmentSettings.LightColor = lightComponent.GetColor();
				break;
			}
		}

		pipeline->BeginRendering();

		for (auto entity : m_Registry.view<entt::entity>())
			OnRender(entity, pipeline);

		// Render all entities
		pipeline->EndRendering();
	}

	void Scene::SetViewportSize(uint32 width, uint32 height)
	{
		if (m_ViewportWidth != width || m_ViewportHeight != height)
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;

			for (auto entity : m_Registry.view<entt::entity>())
				OnViewportResize(entity, width, height);
		}
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

	Entity Scene::CreateCamera(const std::string& name)
	{
		Entity entity = CreateEmpty(name);
		entity.AddComponent<CameraComponent>();
		return entity;
	}

	Entity Scene::CreateDirectionalLight(const std::string& name, const Vector3& rotation)
	{
		Entity entity = CreateEmpty(name);
		entity.AddComponent<LightComponent>().SetLightType(LightComponent::LightType::Directional);
		entity.GetComponent<TransformComponent>().SetLocalRotation(Quaternion(rotation * Math::DegToRad));
		return entity;
	}

	void Scene::CreateSceneEntity()
	{		
		Guid rootEntityGUID = Guid::NewGuid();
		m_SceneEntity = new Entity(m_Registry.create(), this);
		m_EntityMap[rootEntityGUID] = *m_SceneEntity;

		m_SceneEntity->AddComponent<IDComponent>().SetGUID(rootEntityGUID);
		m_SceneEntity->AddComponent<NameComponent>().SetName("Scene");
		m_SceneEntity->AddComponent<RelationshipComponent>();
	}

	void Scene::DestroySceneEntity()
	{
		// TODO: properly destroy scene entity
		delete m_SceneEntity;
		m_SceneEntity = nullptr;
	}

	Entity Scene::GetEntityFromGUID(const Guid& guid)
	{
		auto it = m_EntityMap.find(guid);
		if (it != m_EntityMap.end())
			return it->second;
		return {};
	}

	Entity Scene::GetMainCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
			return { entity, this };
		return {};
	}

	void Scene::OnComponentAdded(Entity entity, Component& component)
	{
		component.SetEntity(entity, this);
		component.OnInit();
		component.OnViewportResize(m_ViewportWidth, m_ViewportHeight);
#if 0
		component.SetChangedCallback([entity](auto component)
		{
			FLUX_WARNING("{0} Component in entity '{1}' was modified!", Utils::ComponentTypeToString(component->GetType()), entity.GetComponent<NameComponent>().GetName());
		});
#endif
	}

}