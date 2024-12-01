#pragma once

#include <set>
#include <bitset>
#include <iostream>
#include <vector>
#include <memory>
#include <typeinfo>
#include "Component.h"
#include "Entity.h"
#include "Pool.h"
#include "src/Utils/Logger.h"
#include <deque>

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

	// The registry Update() process the entities that are waiting to be added/killed
	void Update();

	// Entity Management
	Entity CreateEntity();
	void KillEntity(Entity entity);

	// Component management
	template <typename TComponent, typename... TArgs>
	void AddComponent(Entity entity, TArgs&&... args);
	template <typename TComponent>
	void RemoveComponent(Entity entity);
	template <typename TComponent>
	bool HasComponent(Entity entity) const;
	template <typename TComponent>
	TComponent& GetComponent(Entity entity) const;

	// Check the component signature of an entity and add the entity to the systems that are interested in it
	void AddEntityToSystems(Entity entity) const;
	void RemoveEntityFromSystem(Entity entity);

private:
	int m_numEntities = 0;

	// Vector of component pools, each pool contains all the data for a certain component types
	// [Vector index = component type id]
	// [Pool index = entity id]
	std::vector<std::shared_ptr<IPool>> m_componentPools;

	// Vector of component signatures per entity, saying which component is turned on for a given entity
	// [Vector index = entity id]
	std::vector<Signature> m_entityComponentSignatures;

	// Set of entities that are flagged to be added or removed in the next registry Update();
	std::set<Entity> m_entitiesToBeAdded;
	std::set<Entity> m_entitiesToBeKilled;

	// List of free entity ids that were previously removed
	std::deque<int> m_freeIds;
};

//------------------------------------------------------------------------
// Component management Template Functions
//------------------------------------------------------------------------
template <typename TComponent, typename ...TArgs>
void Coordinator::AddComponent(Entity entity, TArgs&& ...args)
{
	// std::cout << "Printing forwarded arguments:" << std::endl;
	// PrintArgs(std::forward<TArgs>(args)...);

	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	// Ensuring entity doesn't have this component
	/*if (!m_entityComponentSignatures[entityId].test(componentId))
	{*/
	std::cout << "Here" << std::endl;
	// Check if the list of Pools is big enough, if not resize()
	if (componentId >= m_componentPools.size())
	{
		Logger::Warn("Increasing m_componentPools size");
		m_componentPools.resize(componentId + 1, nullptr);
	}

	std::cout << "The type is: " << typeid(TComponent).name() << std::endl;
	std::cout << "Component Id: " << componentId << std::endl;
	std::cout << "Component Pools [Component id]: " << m_componentPools[componentId] << std::endl;
	// if the required pool is not there, add a new pool for the component
	if (!m_componentPools[componentId])
	{
		std::cout << "The type is: " << typeid(TComponent).name() << std::endl;
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		m_componentPools[componentId] = newComponentPool;
	}

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(m_componentPools[componentId]);

	// If the component pool is not big enough for the entity id then resize()
	if (entityId >= componentPool->GetSize())
	{
		std::cout << "Resizing " << typeid(TComponent).name() << " Pool" << std::endl;
		componentPool->Resize(m_numEntities);
	}

	// Finally we can create the component and set it's component bitset
	TComponent newComponent(std::forward<TArgs>(args)...);
	componentPool->Set(entityId, newComponent);
	m_entityComponentSignatures[entityId].set(componentId);

	if (m_entityComponentSignatures[entityId].test(componentId))
	{
		Logger::Log(
			"Component id = " + std::to_string(componentId) +
			" was added to entity id " + std::to_string(entityId));
	}
	else
	{
		Logger::Err("Couldn't add the component!");
	}
	/*}
	else
	{
		Logger::Err("Entity already has this component");
	}*/
}

template <typename TComponent>
void Coordinator::RemoveComponent(Entity entity)
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	// Check if component exist
	if (m_entityComponentSignatures[entityId].test(componentId))
	{
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
bool Coordinator::HasComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();

	return m_entityComponentSignatures[entityId].test(componentId);
}

template<typename TComponent>
TComponent& Coordinator::GetComponent(Entity entity) const
{
	const auto componentId = Component<TComponent>::GetId();
	const auto entityId = entity.GetId();
	auto componentPool = std::static_pointer_cast<Pool<TComponent>>(m_componentPools[componentId]);
	return componentPool->Get(entityId);
}


// // For debug
// template <typename... TArgs>
// void PrintArgs(TArgs&&... args)
// {
// 	// Fold expression to print each argument
// 	((std::cout << typeid(args).name() << " "), ...);
// 	std::cout << std::endl; // Newline at the end
// }