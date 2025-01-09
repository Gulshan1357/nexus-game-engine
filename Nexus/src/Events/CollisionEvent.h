#pragma once

#include <optional>
#include <utility>

#include "src/ECS/Entity.h"
#include "src/EventManagement/IEvent.h"
#include "src/Physics/Contact.h"

/**
 * CollisionEvent
 * @param a (Entity): Entity a (Entity a collides with Entity b)
 * @param b (Entity): Entity b (The 'other' entity)
 * @param contacts (std::vector<Contact>): A vector of contacts for multiple contact points
*/
class CollisionEvent : public IEvent
{
public:
	Entity a;
	Entity b;

	std::vector<Contact> contacts;

	CollisionEvent(const Entity& a, const Entity& b, std::vector<Contact>& contacts) :
		a(a), b(b), contacts(contacts)
	{}
};