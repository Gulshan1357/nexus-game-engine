#include "stdafx.h"
#include "Coordinator.h"

#include <cassert>

#include "../Utils/Logger.h"
#include "Entity.h"
#include "Pool.h"

void Coordinator::Update()
{
	// Logger::Warn("Coordinator::Update() called!");
	// Processing the entities that are waiting to be created to the active System
	for (auto entity : m_entitiesToBeAdded)
	{
		AddEntityToSystems(entity);
	}
	m_entitiesToBeAdded.clear();

	// Processing the entities that are waiting to be killed from the active system
	for (auto entity : m_entitiesToBeKilled)
	{
		RemoveEntityFromSystem(entity);

		m_entityComponentSignatures[entity.GetId()].reset();

		// Make the entity id available to be reused
		m_freeIds.push_back(entity.GetId());
	}
	m_entitiesToBeKilled.clear();

}

Entity Coordinator::CreateEntity()
{
	int entityId;

	/*std::cout << "Free ids: ";
	for (auto id : m_freeIds)
	{
		std::cout << id << "--";
	}
	std::cout << "\n";*/

	if (m_freeIds.empty())
	{
		// If there are no free ids waiting to be reused
		// Logger::Warn("no free ids");
		entityId = m_numEntities++;

		// Ensure that m_entityComponentSignatures doesn't exceed the limit of INT_MAX.
		assert(m_entityComponentSignatures.size() <= static_cast<size_t>(INT_MAX) &&
			"m_entityComponentSignatures size exceeds INT_MAX");

		// Make sure the entityComponentSignatures vector can accommodate the new entity
		if (static_cast<size_t>(entityId) >= m_entityComponentSignatures.size())
		{
			m_entityComponentSignatures.resize(static_cast<size_t>(entityId) + 1);
		}
	}
	else
	{
		// Reuse an id from the list of previously removed entities
		entityId = m_freeIds.front();
		//Logger::Warn("Free id available");
		m_freeIds.pop_front();
	}

	Entity entity(entityId);
	entity.m_Coordinator = this;
	m_entitiesToBeAdded.insert(entity);

	Logger::Log("Entity created with id = " + std::to_string(entityId));
	return entity;
}

void Coordinator::KillEntity(Entity entity)
{
	m_entitiesToBeKilled.insert(entity);
	Logger::Log("Entity " + std::to_string(entity.GetId()) + " was killed!");
}

void Coordinator::AddEntityToSystems(Entity entity) const
{
	const auto entityId = entity.GetId();

	const auto& entityComponentSignature = m_entityComponentSignatures[entityId];

	for (auto& system : systems)
	{
		const auto& systemComponentSignature = system.second->GetComponentSignature();

		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested)
		{
			system.second->AddEntityToSystem(entity);
		}
	}
}

void Coordinator::RemoveEntityFromSystem(Entity entity) const
{
	for (auto system : systems)
	{
		system.second->RemoveEntityFromSystem(entity);
	}
}

//------------------------------------------------------------------------
// Tag Management
//------------------------------------------------------------------------
void Coordinator::TagEntity(Entity entity, const std::string& tag)
{
	m_entityPerTag.emplace(tag, entity);
	m_tagPerEntity.emplace(entity.GetId(), tag);
}

bool Coordinator::EntityHasTag(Entity entity, const std::string& tag) const
{
	if (m_tagPerEntity.find(entity.GetId()) == m_tagPerEntity.end())
	{
		return false;
	}
	return m_entityPerTag.find(tag)->second == entity;
}

Entity Coordinator::GetEntityByTag(const std::string& tag) const
{
	return m_entityPerTag.at(tag);
}

void Coordinator::RemoveEntityTag(Entity entity)
{
	auto taggedEntity = m_tagPerEntity.find(entity.GetId());
	if (taggedEntity != m_tagPerEntity.end())
	{
		auto tag = taggedEntity->second;
		m_entityPerTag.erase(tag);
		m_tagPerEntity.erase(taggedEntity);
	}
}

//------------------------------------------------------------------------
// Group Management
//------------------------------------------------------------------------
void Coordinator::GroupEntity(Entity entity, const std::string& group)
{
	m_entitiesPerGroup.emplace(group, std::set<Entity>());
	m_entitiesPerGroup[group].emplace(entity);
	m_groupPerEntity.emplace(entity.GetId(), group);
}

bool Coordinator::EntityBelongsToGroup(Entity entity, const std::string& group) const
{
	if (m_entitiesPerGroup.find(group) == m_entitiesPerGroup.end())
	{
		return false;
	}
	// auto groupEntities = m_entitiesPerGroup.at(group);
	// return groupEntities.find(entity.GetId()) != groupEntities.end();
	return m_groupPerEntity.find(entity.GetId())->second == group;
}

std::vector<Entity> Coordinator::GetEntitiesByGroup(const std::string& group) const
{
	auto& setOfEntities = m_entitiesPerGroup.at(group);
	return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());

}

void Coordinator::RemoveEntityGroup(Entity entity)
{
	//if in group, remove entity from group management
	auto groupedEntity = m_groupPerEntity.find(entity.GetId());
	if (groupedEntity != m_groupPerEntity.end())
	{
		auto group = m_entitiesPerGroup.find(groupedEntity->second);
		if (group != m_entitiesPerGroup.end())
		{
			auto entityInGroup = group->second.find(entity);
			if (entityInGroup != group->second.end())
			{
				group->second.erase(entityInGroup);
			}
		}
		m_groupPerEntity.erase(groupedEntity);
	}
}
