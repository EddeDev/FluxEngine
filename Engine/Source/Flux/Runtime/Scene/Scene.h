#pragma once

#include "Component.h"

#include "Flux/Runtime/Renderer/RenderPipeline.h"

#include <entt/entt.hpp>

namespace Flux {

	class Entity;

	class Scene : public ReferenceCounted
	{
	public:
		Scene();
		virtual ~Scene();

		void OnUpdate();
		void OnRender(Ref<RenderPipeline> pipeline, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix);

		Entity CreateEmpty(const std::string& name);
		Entity CreateEmpty(const std::string& name, const Guid& guid);
		Entity GetEntityFromGUID(const Guid& guid);
		const Entity& GetRootEntity() const { return *m_SceneEntity; }

		bool RemoveChild(Entity child);

		entt::registry& GetRegistry() { return m_Registry; }
		const entt::registry& GetRegistry() const { return m_Registry; }
	private:
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
	private:
		entt::registry m_Registry;
		std::unordered_map<Guid, Entity> m_EntityMap;

		Entity* m_SceneEntity = nullptr;
	};

}