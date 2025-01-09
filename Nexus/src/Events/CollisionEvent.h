#pragma once

#include <optional>

#include "src/ECS/Entity.h"
#include "src/EventManagement/IEvent.h"
#include "src/Physics/Contact.h"

/**
 * CollisionEvent
 * @param a (Entity): Entity a (Entity a collides with Entity b)
 * @param b (Entity): Entity b (The 'other' entity)
*/
class CollisionEvent : public IEvent
{
public:
	Entity a;
	Entity b;

	Contact contact;

	CollisionEvent(const Entity& a, const Entity& b, Contact contact) :
		a(a), b(b), contact(contact)
	{}
};