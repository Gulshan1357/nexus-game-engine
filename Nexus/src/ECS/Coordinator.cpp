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

		// Remove any traces of that entity from the tag/group maps
		RemoveEntityTag(entity);
		RemoveEntityGroup(entity);

		// Remove all relationships
		RemoveAllRelationships(entity);

		// Make the entity id available to be reused
		m_freeIds.push_back(entity.GetId());
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
	const auto it = m_entityPerTag.find(tag);
	return (it != m_entityPerTag.end() && it->second == entity);
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
	// Check if the entity exists in m_groupPerEntity
	const auto it = m_groupPerEntity.find(entity.GetId());
	if (it == m_groupPerEntity.end())
	{
		return false; // Entity is not part of any group
	}
	// Check if the entity's group matches the provided group
	return it->second == group;
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

//------------------------------------------------------------------------
// Check the component signature of an entity and add the entity to the systems that are interested in it
//------------------------------------------------------------------------
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
// Relationship Management
//------------------------------------------------------------------------
void Coordinator::AddRelationship(Entity source, Entity target, const std::string& relationshipTag)
{
	m_relationships.emplace(source, std::make_pair(target, relationshipTag));
	Logger::Log("Added relationship: " + relationshipTag +
		" from entity " + std::to_string(source.GetId()) +
		" to entity " + std::to_string(target.GetId()));
}

void Coordinator::RemoveRelationship(Entity source, Entity target, const std::string& relationshipTag)
{
	const auto [first, last] = m_relationships.equal_range(source);
	for (auto it = first; it != last; ++it)
	{
		// it = [entity, relationshipPair<entity, relationshipTag>] = [leader, <follower, relationship>]
		if (it->second.first == target && it->second.second == relationshipTag)
		{
			m_relationships.erase(it);
			Logger::Log("Removed relationship: " + relationshipTag +
				" from entity " + std::to_string(source.GetId()) +
				" to entity " + std::to_string(target.GetId()));
			return;
		}
	}
	Logger::Err("Failed to remove relationship: Not found");
}

std::vector<std::pair<Entity, std::string>> Coordinator::GetRelationships(Entity source) const
{
	std::vector<std::pair<Entity, std::string>> relationships;
	const auto [first, last] = m_relationships.equal_range(source);
	for (auto it = first; it != last; ++it)
	{
		// it = [entity, relationshipPair<entity, relationshipTag>] = [leader, <follower, relationship>]
		// Push <follower, relationship> to relationships vector
		relationships.push_back(it->second);
	}
	return relationships;
}

bool Coordinator::HasRelationship(Entity source, Entity target, const std::string& relationshipTag) const
{
	const auto [first, last] = m_relationships.equal_range(source);
	for (auto it = first; it != last; ++it)
	{
		// // it = [entity, relationshipPair<entity, relationshipTag>] = [leader, <follower, relationship>]
		if (it->second.first == target && it->second.second == relationshipTag)
		{
			return true;
		}
	}
	return false;
}

std::vector<Entity> Coordinator::GetEntitiesByRelationshipTag(const Entity& source, const std::string& relationshipTag) const
{
	std::vector<Entity> entities;

	// Iterate through all relationships in the map
	const auto [first, last] = m_relationships.equal_range(source);  // Get the relationships for the source entity
	for (auto it = first; it != last; ++it)
	{
		// it = [entity, relationshipPair<entity, relationshipTag>] = [leader, <follower, relationship>]
		if (it->second.second == relationshipTag)
		{
			entities.push_back(it->second.first);
		}
	}
	return entities;
}

void Coordinator::RemoveAllRelationships(Entity entity)
{
	for (auto it = m_relationships.begin(); it != m_relationships.end(); )
	{
		bool removed = false;

		// Check if the entity is the leader (source)
		if (it->first == entity)
		{
			Logger::Log("Removed relationship: " + it->second.second +
				" from entity " + std::to_string(entity.GetId()) +
				" to entity " + std::to_string(it->second.first.GetId()));
			it = m_relationships.erase(it); // erase and move to the next element
			removed = true;
		}
		// Check if the entity is the follower (target)
		else if (it->second.first == entity)
		{
			Logger::Log("Removed relationship: " + it->second.second +
				" from entity " + std::to_string(it->first.GetId()) +
				" to entity " + std::to_string(entity.GetId()));
			it = m_relationships.erase(it); // erase and move to the next element
			removed = true;
		}

		if (!removed)
		{
			++it;
		}
	}
}
