#pragma once

#include "src/Utils/Vector2.h"

/**
 * Contact
 * @param startContactPoint (Vector2): Start contact point (From "a" to "b")
 * @param endContactPoint (Vector2): End contact point (From "a" to "b")
 * @param collisionNormal (Vector2): Normal Direction from "a" to "b"
 * @param penetrationDepth (Float): The penetration length of the collision
*/
struct Contact
{
	Vector2 startContactPoint;
	Vector2 endContactPoint;
	Vector2 collisionNormal;
	float penetrationDepth;

	Contact(const Vector2& startContactPoint = Vector2(), const Vector2& endContactPoint = Vector2(), const Vector2& collisionNormal = Vector2(), float penetrationDepth = 0.0f) :
		startContactPoint(startContactPoint), endContactPoint(endContactPoint), collisionNormal(collisionNormal), penetrationDepth(penetrationDepth)
	{}
};