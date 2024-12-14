#include "stdafx.h"
#include "Entity.h"

#include "Coordinator.h"

size_t Entity::GetId() const { return m_id; }

void Entity::Kill() const
{
	coordinator->KillEntity(*this);
}

void Entity::Tag(const std::string& tag) const
{
	coordinator->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string& tag) const
{
	return coordinator->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string& group) const
{
	coordinator->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const std::string& group) const
{
	return coordinator->EntityBelongsToGroup(*this, group);
}
