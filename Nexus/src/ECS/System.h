#pragma once
#include <vector>
#include <bitset>

#include "Component.h"

// TODO: Refactor the magic number from coordinator
using Signature = std::bitset<32>;
class Entity;

class System
{
public:
	void AddEntityToSystem(Entity entity);
	void RemoveEntityFromSystem(Entity entity);
	std::vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;

	// Define the component type T that the entities must have to be considered by the system
	template <typename TComponent>
	void RequireComponent();

private:
	std::vector<Entity> m_entities;
	Signature m_componentSignature;
};

template <typename TComponent>
void System::RequireComponent()
{
	const auto componentId = Component<TComponent>::GetId();
	m_componentSignature.set(componentId);
}
