#pragma once

#include "Component.h"

#include "Flux/Runtime/Asset/Asset.h"
#include "Flux/Runtime/Renderer/RenderPipeline.h"

#include <entt/entt.hpp>

namespace Flux {

	class Entity;

	// TODO: rename this?
	struct SceneCameraData
	{
		Matrix4x4 ViewMatrix = Matrix4x4(1.0f);
		Matrix4x4 ProjectionMatrix = Matrix4x4(1.0f);
		Matrix4x4 ViewProjectionMatrix = Matrix4x4(1.0f);
		Matrix4x4 InverseViewProjectionMatrix = Matrix4x4(1.0f);
		Vector3 Position = Vector3(0.0f);
		float NearClip = 0.1f;
		float FarClip = 1000.0f;
	};

	class Scene : public Asset
	{
	public:
		Scene();
		virtual ~Scene();

		void OnUpdate();
		void OnRender(Ref<RenderPipeline> pipeline);
		void OnRender(Ref<RenderPipeline> pipeline, const SceneCameraData& cameraData);
		void SetViewportSize(uint32 width, uint32 height);

		Entity CreateEmpty(const std::string& name);
		Entity CreateEmpty(const std::string& name, const Guid& guid);
		Entity CreateCamera(const std::string& name);
		Entity CreateDirectionalLight(const std::string& name, const Vector3& rotation);
		Entity GetEntityFromGUID(const Guid& guid);
		const Entity& GetRootEntity() const { return *m_SceneEntity; }

		Entity GetMainCameraEntity();

		entt::registry& GetRegistry() { return m_Registry; }
		const entt::registry& GetRegistry() const { return m_Registry; }

		ASSET_CLASS_TYPE(Scene)
	private:
		void CreateSceneEntity();
		void DestroySceneEntity();

		void OnComponentAdded(Entity entity, Component& component);

		template<typename T>
		void OnComponentAdded(entt::registry& registry, entt::entity entity)
		{
			OnComponentAdded({ entity, this }, dynamic_cast<Component&>(registry.get<T>(entity)));
		}

		template<typename... Component>
		void RegisterComponentCallbacks(entt::registry& registry)
		{
			([&]()
			{
				registry.on_construct<Component>().connect<&Scene::OnComponentAdded<Component>>(this);
			}(), ...);
		}

		template<typename... Component>
		void RegisterComponentCallbacks(ComponentSet<Component...>, entt::registry& registry)
		{
			RegisterComponentCallbacks<Component...>(registry);
		}

		void RegisterComponentCallbacks(entt::registry& registry)
		{
			RegisterComponentCallbacks(AllComponents{}, registry);
		}

		template<typename... Component>
		void OnUpdate(entt::entity entity)
		{
			([&]()
			{
				Component* component = m_Registry.try_get<Component>(entity);
				if (component)
					component->OnUpdate();
			}(), ...);
		}

		template<typename... Component>
		void OnUpdate(ComponentSet<Component...>, entt::entity entity)
		{
			OnUpdate<Component...>(entity);
		}

		void OnUpdate(entt::entity entity)
		{
			OnUpdate(AllComponents{}, entity);
		}

		template<typename... Component>
		void OnRender(entt::entity entity, Ref<RenderPipeline> pipeline)
		{
			([&]()
			{
				Component* component = m_Registry.try_get<Component>(entity);
				if (component)
					component->OnRender(pipeline);
			}(), ...);
		}

		template<typename... Component>
		void OnRender(ComponentSet<Component...>, entt::entity entity, Ref<RenderPipeline> pipeline)
		{
			OnRender<Component...>(entity, pipeline);
		}

		void OnRender(entt::entity entity, Ref<RenderPipeline> pipeline)
		{
			OnRender(AllComponents{}, entity, pipeline);
		}

		template<typename... Component>
		void OnViewportResize(entt::entity entity, uint32 width, uint32 height)
		{
			([&]()
			{
				Component* component = m_Registry.try_get<Component>(entity);
				if (component)
					component->OnViewportResize(width, height);
			}(), ...);
		}

		template<typename... Component>
		void OnViewportResize(ComponentSet<Component...>, entt::entity entity, uint32 width, uint32 height)
		{
			OnViewportResize<Component...>(entity, width, height);
		}

		void OnViewportResize(entt::entity entity, uint32 width, uint32 height)
		{
			OnViewportResize(AllComponents{}, entity, width, height);
		}
	private:
		entt::registry m_Registry;
		std::unordered_map<Guid, Entity> m_EntityMap;

		Entity* m_SceneEntity = nullptr;

		uint32 m_ViewportWidth = 0;
		uint32 m_ViewportHeight = 0;
	};

}