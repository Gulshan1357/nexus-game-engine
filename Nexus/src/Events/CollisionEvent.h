#pragma once

#include "src/ECS/Entity.h"
#include "src/EventManagement/IEvent.h"

class CollisionEvent : public IEvent
{
public:
	Entity a;
	Entity b;
	CollisionEvent(const Entity a, const Entity b) : a(a), b(b) {}
};