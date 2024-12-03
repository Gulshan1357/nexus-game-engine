#include "stdafx.h"
#include "Entity.h"
#include "Coordinator.h"

size_t Entity::GetId() const { return m_id; }

void Entity::Kill() const
{
	m_Coordinator->KillEntity(*this);
}

void Entity::Tag(const std::string& tag) const
{
	m_Coordinator->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string& tag) const
{
	return m_Coordinator->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string& group) const
{
	m_Coordinator->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const std::string& group) const
{
	return m_Coordinator->EntityBelongsToGroup(*this, group);
}
