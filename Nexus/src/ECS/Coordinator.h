#pragma once

#include <set>
#include <bitset>
#include <vector>
#include <memory>
#include <deque>
#include <typeinfo>
#include <unordered_map>
#include <typeindex>
#include <utility>

#include "Component.h"
#include "Entity.h"
#include "System.h"
#include "Pool.h"

#include "src/Utils/Logger.h"

// Do I need to forward declare IPool?
class IPool;

constexpr unsigned int MAX_COMPONENTS = 32;
using Signature = std::bitset<MAX_COMPONENTS>;

class Coordinator
{
public:
	Coordinator()
	{
		Logger::Log("Coordinator constructor called");
	}
	~Coordinator()
	{
		Logger::Log("Coordinator destructor called");
	}

	// The m_Coordinator Update() process the entities that are waiting to be added/killed
	void Update();

	// Entity Management
	Entity CreateEntity();
	void KillEntity(Entity entity);

	// Tag management
	void TagEntity(Entity entity, const std::string& tag);
	[[nodiscard]] bool EntityHasTag(Entity entity, const std::string& tag) const;
	Entity& GetEntityByTag(const std::string& tag);
	void RemoveEntityTag(Entity entity);

	// Group management
	void GroupEntity(Entity entity, const std::string& group);
	[[nodiscard]] bool EntityBelongsToGroup(Entity entity, const std::string& group) const;
	[[nodiscard]] std::vector<Entity> GetEntitiesByGroup(const std::string& group) const;
	void RemoveEntityGroup(Entity entity);

	// Component management
	template <typename TComponent, typename... TArgs>
	void AddComponent(Entity entity, TArgs&&... args);
	template <typename TComponent>
	void RemoveComponent(Entity entity);
	template <typename TComponent>
	[[nodiscard]] bool HasComponent(Entity entity) const;
	template <typename TComponent>
	TComponent& GetComponent(Entity entity);

	// System management
	template <typename TSystem, typename... TArgs>
	void AddSystem(TArgs&&... args);
	template <typename TSystem>
	void RemoveSystem();
	template <typename TSystem>
	[[nodiscard]] bool HasSystem() const;
	template <typename TSystem>
	TSystem& GetSystem() const;

	// Check the component signature of an entity and add the entity to the systems that are interested in it
	void AddEntityToSystems(Entity entity) const;
	void RemoveEntityFromSystem(Entity entity) const;

	// Entity(one) - Entity(many) Relationship Management
	void AddRelationship(Entity source, Entity target, const std::string& relationshipTag);
	void RemoveRelationship(Entity source, Entity target, const std::string& relationshipTag);
	[[nodiscard]] std::vector<std::pair<Entity, std::string>> GetRelationships(Entity source) const;
	[[nodiscard]] bool HasRelationship(Entity source, Entity target, const std::string& relationshipTag) const;
	[[nodiscard]] std::vector<Entity> GetEntitiesByRelationshipTag(const Entity& source, const std::string& relationshipTag) const;
	void RemoveAllRelationships(Entity entity); // To remove all relationships from an entity

private:
	size_t m_numEntities = 0;

	// Vector of component pools, each pool contains all the data for a certain component types
	// [Vector index = component type id]
	// [Pool index = entity id]
	std::vector<std::shared_ptr<IPool>> m_componentPools;

	// Vector of component signatures per entity, saying which component is turned on for a given entity
	// [Vector index = entity id]
	std::vector<Signature> m_entityComponentSignatures;

	// Map of active systems
	// [Map key = system type id]
	std::unordered_map<std::type_index, std::shared_ptr<System>> m_systems;

	// Set of entities that are flagged to be added or removed in the next m_Coordinator Update();
	std::set<Entity> m_entitiesToBeAdded;
	std::set<Entity> m_entitiesToBeKilled;

	// TODO: To ensure faster lookup both ways for Tags and Groups, creating duplicates. Maybe find an alternative which doesn't sacrifice the performance

	// Each entity can only have one tag and vice-versa
	// Entity tags (one tag name per entity) 
	std::unordered_map<std::string, Entity> m_entityPerTag;
	std::unordered_map<size_t, std::string> m_tagPerEntity;

	// Each group can have multiple entities but an entity can only belong to one group
	//Entity groups (a set of entities per group name) 
	std::unordered_map<std::string, std::set<Entity>> m_entitiesPerGroup;
	std::unordered_map<size_t, std::string> m_groupPerEntity;
	// TODO: Entity should be able to belong to multiple groups

	// List of free entity ids that were previously removed
	std::deque<size_t> m_freeIds;

	// Map to manage relationships between entities
	std::unordered_multimap<Entity, std::pair<Entity, std::string>> m_relationships;
};

//------------------------------------------------------------------------
// Component management Template Functions
//------------------------------------------------------------------------
template <typename TComponent, typename ...TArgs>
void Coordinator::AddComponent(const Entity entity, TArgs&& ...args)
{
	// std::cout << "Printing forwarded arguments:" << std::endl;
	// PrintArgs(std::forward<TArgs>(args)...);

	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	// Ensuring entity doesn't have this component
	if (!m_entityComponentSignatures[entityId].test(componentId))
	{
		// std::cout << "Here" << std::endl;
		// Check if the list of Pools is big enough, if not resize()
		if (componentId >= m_componentPools.size())
		{
			// Logger::Warn("Increasing m_componentPools size");
			m_componentPools.resize(componentId + 1, nullptr);
		}

		// std::cout << "The type is: " << typeid(TComponent).name() << std::endl;
		// std::cout << "Component Id: " << componentId << std::endl;
		// std::cout << "Component Pools [Component id]: " << m_componentPools[componentId] << std::endl;
		// if the required pool is not there, add a new pool for the component
		if (!m_componentPools[componentId])
		{
			// std::cout << "The type is: " << typeid(TComponent).name() << std::endl;
			std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
			m_componentPools[componentId] = newComponentPool;
		}

		std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(m_componentPools[componentId]);

		// If the component pool is not big enough for the entity id then resize()
		if (entityId >= componentPool->GetSize())
		{
			// std::cout << "Resizing " << typeid(TComponent).name() << " Pool" << std::endl;
			componentPool->Resize(m_numEntities);
		}

		// Finally we can create the component and set it's component bitset
		TComponent newComponent(std::forward<TArgs>(args)...);
		componentPool->Set(entityId, newComponent);
		m_entityComponentSignatures[entityId].set(componentId);

		if (m_entityComponentSignatures[entityId].test(componentId))
		{
			// Logger::Log(
			// 	"Component id = " + std::to_string(componentId) +
			// 	" was added to entity id " + std::to_string(entityId));
			// std::cout <<  typeid(TComponent).name() << " Pool with Component id: " << componentId << "--- Pool Size: " << componentPool->GetSize() << "\n";

			// std::cout <<
			// 	"Component name = " <<
			// 	typeid(TComponent).name() <<
			// 	" Component id = " <<
			// 	std::to_string(componentId) <<
			// 	" was added to entity id " <<
			// 	std::to_string(entityId) <<
			// 	" --- Pool Size: " <<
			// 	componentPool->GetSize() << "\n";
		}
		else
		{
			Logger::Err("Couldn't add the component!");
		}
	}
	else
	{
		Logger::Err("Entity already has this component");
	}
}

template <typename TComponent>
void Coordinator::RemoveComponent(const Entity entity)
{
	const auto componentId = Component<TComponent>::GetId();

	// Check if component exist
	if (const auto entityId = entity.GetId(); m_entityComponentSignatures[entityId].test(componentId))
	{
		// Remove the component from the component list for that entity
		std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(m_componentPools[componentId]);
		componentPool->Remove(entityId);

		// Set this component signature for that entity to false
		m_entityComponentSignatures[entityId].set(componentId, false);
		Logger::Log(
			"Component id = " + std::to_string(componentId) + " was removed to entity id " + std::to_string(entityId));
	}
	else
	{
		Logger::Err("Couldn't remove component: It doesn't exist.");
	}
}

template <typename TComponent>
bool Coordinator::HasComponent(const Entity entity) const
{
	const auto& componentId = Component<TComponent>::GetId();
	const auto& entityId = entity.GetId();

	return m_entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Coordinator::GetComponent(Entity entity)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(m_componentPools[componentId]);
	return componentPool->Get(entityId);
}

//------------------------------------------------------------------------
// System management Template Functions
//------------------------------------------------------------------------
template <typename TSystem, typename ...TArgs>
void Coordinator::AddSystem(TArgs&& ...args)
{
	// TSystem* newSystem(new TSystem(std::forward<TArgs>(args)...));
	std::unique_ptr<TSystem> newSystem(std::make_unique<TSystem>(std::forward<TArgs>(args)...));
	m_systems.insert(std::make_pair(std::type_index(typeid(TSystem)), std::move(newSystem)));
}

template <typename TSystem>
void Coordinator::RemoveSystem()
{
	const auto system = m_systems.find(std::type_index(typeid(TSystem)));
	m_systems.erase(system);
}

template <typename TSystem>
bool Coordinator::HasSystem() const
{
	return m_systems.find(std::type_index(typeid(TSystem))) != m_systems.end();
}

//TODO: change the following to be more memory safe. Use share pointer or reference to unique pointer
template <typename TSystem>
TSystem& Coordinator::GetSystem() const
{
	const auto system = m_systems.find(std::type_index(typeid(TSystem)));
	return *(static_cast<TSystem*>(system->second.get()));
}

// // To debug forward Args
// template <typename... TArgs>
// void PrintArgs(TArgs&&... args)
// {
// 	// Fold expression to print each argument
// 	((std::cout << typeid(args).name() << " "), ...);
// 	std::cout << std::endl; // Newline at the end
// }