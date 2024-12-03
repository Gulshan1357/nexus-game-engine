#pragma once

#include "../ECS/Entity.h"
#include "../Event/IEvent.h"

class CollisionEvent : public IEvent
{
public:
	Entity a;
	Entity b;
	CollisionEvent(Entity a, Entity b) : a(a), b(b) {}
};