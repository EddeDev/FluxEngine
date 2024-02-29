#include "FluxPCH.h"
#include "Entity.h"

#include "Scene.h"

namespace Flux {

	Entity::Entity()
		: m_Entity(entt::null), m_Scene(nullptr)
	{
	}

	Entity::Entity(entt::entity entity, Scene* scene)
		: m_Entity(entity), m_Scene(scene)
	{
	}

	const Guid& Entity::GetGUID() const
	{
		return GetComponent<IDComponent>().GetGUID();
	}

	const Guid& Entity::GetParentGUID() const
	{
		return GetComponent<RelationshipComponent>().GetParent();
	}

	void Entity::SetParentGUID(const Guid& guid)
	{
		GetComponent<RelationshipComponent>().SetParent(guid);
	}

	void Entity::SetParent(Entity parent)
	{
		if (!parent)
			parent = m_Scene->GetRootEntity();

		Entity previousParent = GetParent();
		if (previousParent == parent)
			return;

		auto& childRelationship = GetComponent<RelationshipComponent>();
		auto& childGUID = GetGUID();

		if (previousParent)
		{
			auto& previousParentRelationship = previousParent.GetComponent<RelationshipComponent>();
			previousParentRelationship.DecrementChildCount();

			auto& previousChildGUID = childRelationship.GetPrevious();
			auto& nextChildGUID = childRelationship.GetNext();

			if (previousChildGUID)
			{
				Entity previousChild = m_Scene->GetEntityFromGUID(previousChildGUID);
				previousChild.GetComponent<RelationshipComponent>().SetNext(nextChildGUID);
			}

			if (nextChildGUID)
			{
				Entity nextChild = m_Scene->GetEntityFromGUID(nextChildGUID);
				nextChild.GetComponent<RelationshipComponent>().SetPrevious(previousChildGUID);
			}

			if (previousParentRelationship.GetFirstChild() == childGUID)
				previousParentRelationship.SetFirstChild(childRelationship.GetNext());

			childRelationship.SetParent(m_Scene->GetRootEntity().GetGUID());
			childRelationship.SetPrevious({});
			childRelationship.SetNext({});
		}

		auto& newParentRelationship = parent.GetComponent<RelationshipComponent>();

		childRelationship.SetParent(parent.GetGUID());

		auto& firstChildGUID = newParentRelationship.GetFirstChild();
		if (!firstChildGUID)
		{
			newParentRelationship.SetFirstChild(childGUID);
		}
		else
		{
			Guid lastChildGUID = firstChildGUID;
			for (uint32 i = 0; i < newParentRelationship.GetChildCount() - 1; i++)
				lastChildGUID = m_Scene->GetEntityFromGUID(lastChildGUID).GetComponent<RelationshipComponent>().GetNext();

			if (lastChildGUID)
			{
				Entity lastChild = m_Scene->GetEntityFromGUID(lastChildGUID);
				lastChild.GetComponent<RelationshipComponent>().SetNext(childGUID);
			}

			childRelationship.SetPrevious(lastChildGUID);
		}

		newParentRelationship.IncrementChildCount();

		auto& transformComponent = GetComponent<TransformComponent>();
		transformComponent.RecalculateTransform();
	}

	Entity Entity::GetParent() const
	{
		return m_Scene->GetEntityFromGUID(GetParentGUID());
	}

	bool Entity::HasParent() const
	{
		return GetParentGUID().IsValid();
	}

	bool Entity::HasChildren() const
	{
		return GetComponent<RelationshipComponent>().GetChildCount() > 0;
	}

	bool Entity::IsParentOf(Entity entity)
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

	bool Entity::IsChildOf(Entity entity)
	{
		return entity.IsParentOf(*this);
	}

	std::vector<Guid> Entity::GetChildrenGUIDs() const
	{
		auto& relationshipComponent = GetComponent<RelationshipComponent>();
		std::vector<Guid> result(relationshipComponent.GetChildCount());
		Guid currentGuid = relationshipComponent.GetFirstChild();
		for (uint32 i = 0; i < relationshipComponent.GetChildCount(); i++)
		{
			result[i] = currentGuid;
			currentGuid = m_Scene->GetEntityFromGUID(currentGuid).GetComponent<RelationshipComponent>().GetNext();
		}
		return result;
	}

	std::vector<Entity> Entity::GetChildren()
	{
		std::vector<Guid> childrenGUIDs = GetChildrenGUIDs();

		std::vector<Entity> result(childrenGUIDs.size());
		for (size_t i = 0; i < childrenGUIDs.size(); i++)
			result[i] = m_Scene->GetEntityFromGUID(childrenGUIDs[i]);
		return result;
	}

}