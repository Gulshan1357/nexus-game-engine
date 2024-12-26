#pragma once

#include <optional>

#include "src/ECS/Entity.h"
#include "src/EventManagement/IEvent.h"

#include "src/Utils/Vector2.h"

/**
 * ContactInfo
 * @param startContactPoint (Vector2): Start contact point (From "a" to "b")
 * @param endContactPoint (Vector2): End contact point (From "a" to "b")
 * @param collisionNormal (Vector2): Normal Direction from "a" to "b"
 * @param penetrationDepth (Float): The penetration length of the collision
*/
struct ContactInfo
{
	Vector2 startContactPoint;
	Vector2 endContactPoint;
	Vector2 collisionNormal;
	float penetrationDepth;

	ContactInfo(const Vector2& startContactPoint = Vector2(), const Vector2& endContactPoint = Vector2(), const Vector2& collisionNormal = Vector2(), float penetrationDepth = 0.0f) :
		startContactPoint(startContactPoint), endContactPoint(endContactPoint), collisionNormal(collisionNormal), penetrationDepth(penetrationDepth)
	{}
};

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

	ContactInfo contact;

	CollisionEvent(const Entity a, const Entity b, ContactInfo contact) :
		a(a), b(b), contact(contact)
	{}
};