#include "stdafx.h"
#include "Entity.h"
#include "Coordinator.h"

int Entity::GetId() const { return m_id; }

void Entity::Kill() const
{
	m_Coordinator->KillEntity(*this);
}
