#include "stdafx.h"
#include "Coordinator.h"

#include <cassert>

#include "Entity.h"
#include "Pool.h"

#include "src/Utils/Logger.h"

void Coordinator::Update()
{
	// Logger::Warn("Coordinator::Update() called!");
	// Processing the entities that are waiting to be created to the active System
	for (const auto entity : m_entitiesToBeAdded)
	{
		AddEntityToSystems(entity);
	}
	m_entitiesToBeAdded.clear();

	// Processing the entities that are waiting to be killed from the active system
	for (auto entity : m_entitiesToBeKilled)
	{
		RemoveEntityFromSystem(entity);

		m_entityComponentSignatures[entity.GetId()].reset();

		// Remove the entity from the component pools
		for (const auto& pool : m_componentPools)
		{
			if (pool)
			{
				pool->RemoveEntityFromPool(entity.GetId());
			}
		}

		// Make the entity id available to be reused
		m_freeIds.push_back(entity.GetId());

		// Remove any traces of that entity from the tag/group maps
		RemoveEntityTag(entity);
		RemoveEntityGroup(entity);
	}
	m_entitiesToBeKilled.clear();

}

Entity Coordinator::CreateEntity()
{
	size_t entityId;

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
		if (entityId >= m_entityComponentSignatures.size())
		{
			m_entityComponentSignatures.resize(entityId + 1);
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
	entity.coordinator = this;
	m_entitiesToBeAdded.insert(entity);

	// Logger::Log("Entity created with id = " + std::to_string(entityId));
	return entity;
}

void Coordinator::KillEntity(const Entity entity)
{
	m_entitiesToBeKilled.insert(entity);
	Logger::Log("Entity " + std::to_string(entity.GetId()) + " was killed!");
}

void Coordinator::AddEntityToSystems(const Entity entity) const
{
	const auto entityId = entity.GetId();

	const auto& entityComponentSignature = m_entityComponentSignatures[entityId];

	for (const auto& [type, system] : m_systems)
	{
		// If interested
		if (const auto& systemComponentSignature = system->GetComponentSignature(); (entityComponentSignature & systemComponentSignature) == systemComponentSignature)
		{
			system->AddEntityToSystem(entity);
		}
	}
}

void Coordinator::RemoveEntityFromSystem(Entity entity) const
{
	for (const auto& [type, system] : m_systems)
	{
		system->RemoveEntityFromSystem(entity);
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

void Coordinator::RemoveEntityTag(const Entity entity)
{
	if (const auto taggedEntity = m_tagPerEntity.find(entity.GetId()); taggedEntity != m_tagPerEntity.end())
	{
		const auto tag = taggedEntity->second;
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
	return { setOfEntities.begin(), setOfEntities.end() };
}

void Coordinator::RemoveEntityGroup(Entity entity)
{
	//if in group, remove entity from group management
	if (const auto groupedEntity = m_groupPerEntity.find(entity.GetId()); groupedEntity != m_groupPerEntity.end())
	{
		if (const auto group = m_entitiesPerGroup.find(groupedEntity->second); group != m_entitiesPerGroup.end())
		{
			const auto entityInGroup = group->second.find(entity);
			if (entityInGroup != group->second.end())
			{
				group->second.erase(entityInGroup);
			}
		}
		m_groupPerEntity.erase(groupedEntity);
	}
}
