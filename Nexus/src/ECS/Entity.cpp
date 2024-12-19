#include "stdafx.h"
#include "Entity.h"

#include "Coordinator.h"

size_t Entity::GetId() const { return m_id; }

void Entity::Kill() const
{
	coordinator->KillEntity(*this);
}

//------------------------------------------------------------------------
// Tag and Group Management
//------------------------------------------------------------------------
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

//------------------------------------------------------------------------
// Relationship Management
//------------------------------------------------------------------------
void Entity::AddRelationship(const Entity target, const std::string& relationshipTag) const
{
	coordinator->AddRelationship(*this, target, relationshipTag);
}

void Entity::RemoveRelationship(const Entity target, const std::string& relationshipTag) const
{
	coordinator->RemoveRelationship(*this, target, relationshipTag);
}

std::vector<std::pair<Entity, std::string>> Entity::GetRelationships() const
{
	return coordinator->GetRelationships(*this);
}

bool Entity::HasRelationship(const Entity target, const std::string& relationshipTag) const
{
	return coordinator->HasRelationship(*this, target, relationshipTag);
}

std::vector<Entity> Entity::GetEntitiesByRelationshipTag(const std::string& relationshipTag) const
{
	return coordinator->GetEntitiesByRelationshipTag(*this, relationshipTag);
}
