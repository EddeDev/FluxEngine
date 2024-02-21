#include "FluxPCH.h"
#include "Entity.h"

#include "Scene.h"

namespace Flux {

	Entity::Entity()
		: m_EntityID(NullEntity), m_Scene(nullptr)
	{
	}

	Entity::Entity(EntityID entityID, Scene* scene)
		: m_EntityID(entityID), m_Scene(scene)
	{
	}

	std::vector<Component*> Entity::GetComponents() const
	{
		return m_Scene->GetRegistry().GetComponents<Component>(m_EntityID);
	}

	void Entity::SetParent(Entity parent)
	{
		if (!parent)
		{
			Unparent();
			return;
		}

		if (parent.IsChildOf(*this))
		{
			parent.Unparent();

			Entity newParent = GetParent();
			if (newParent)
			{
				Unparent();
				parent.SetParent(newParent);
			}
		}
		else
		{
			Unparent();
		}

		SetParentID(parent);
		
		parent.AddChild(*this);
	}

	void Entity::AddChild(Entity child)
	{
		auto& entityData = m_Scene->GetEntityData(m_EntityID);
		FLUX_VERIFY(std::find(entityData.Children.begin(), entityData.Children.end(), child) == entityData.Children.end());
		entityData.Children.push_back(child);
	}

	bool Entity::RemoveChild(Entity child)
	{
		auto& entityData = m_Scene->GetEntityData(m_EntityID);

		if (auto it = std::find(entityData.Children.begin(), entityData.Children.end(), child); it != entityData.Children.end())
		{
			entityData.Children.erase(it);
			return true;
		}

		return false;
	}

	void Entity::Unparent()
	{
		Entity parent = GetParent();
		if (!parent)
			return;

		parent.RemoveChild(*this);

		SetParentID(NullEntity);
	}

	void Entity::SetParentID(EntityID entityID)
	{
		auto& entityData = m_Scene->GetEntityData(m_EntityID);
		entityData.Parent = entityID;
	}

	bool Entity::IsParentOf(Entity entity) const
	{
		std::vector<Entity> children = GetChildren();
		if (children.empty())
			return false;

		for (Entity child : children)
		{
			if (child == entity)
				return true;
		}

		for (Entity child : children)
		{
			if (child.IsParentOf(entity))
				return true;
		}

		return false;
	}

	bool Entity::IsChildOf(Entity entity) const
	{
		return entity.IsParentOf(*this);
	}

	void Entity::SetName(const std::string& name) const
	{
		m_Scene->GetEntityData(m_EntityID).Name = name;
	}

	const std::string& Entity::GetName() const
	{
		return m_Scene->GetEntityData(m_EntityID).Name;
	}

	Entity Entity::GetParent() const
	{
		EntityID parentID = m_Scene->GetEntityData(m_EntityID).Parent;
		if (parentID == NullEntity)
			return {};

		return { parentID, m_Scene };
	}

	bool Entity::HasParent() const
	{
		EntityID parentID = m_Scene->GetEntityData(m_EntityID).Parent;
		if (parentID == NullEntity)
			return false;

		return true;
	}

	bool Entity::HasChildren() const
	{
		auto& entityData = m_Scene->GetEntityData(m_EntityID);
		return !entityData.Children.empty();
	}

	std::vector<Entity> Entity::GetChildren() const
	{
		auto& entityData = m_Scene->GetEntityData(m_EntityID);
		
		std::vector<Entity> result(entityData.Children.size());
		for (size_t i = 0; i < entityData.Children.size(); i++)
			result[i] = { entityData.Children[i], m_Scene };
		return result;
	}

}