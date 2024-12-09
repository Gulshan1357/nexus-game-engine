#include "stdafx.h"
#include "System.h"

#include "Entity.h"

void System::AddEntityToSystem(Entity entity)
{
	m_entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
	m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [&entity](Entity other)
		{
			return entity == other;
		}), m_entities.end());
}

std::vector<Entity> System::GetSystemEntities() const
{
	return m_entities;
}

const Signature& System::GetComponentSignature() const
{
	return m_componentSignature;
}
