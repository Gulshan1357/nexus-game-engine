#pragma once
#include "src/Utils/Vector2.h"

/**
 * CircleColliderComponent Component provides information for Collision System
 * @param radius (float)
 * @param offset (Vector2)
*/
struct CircleColliderComponent
{
	float radius;
	Vector2 offset;

	Vector2 globalCenter;

	explicit CircleColliderComponent(const float radius = 1.f, const Vector2 offset = Vector2()) :
		radius(radius), offset(offset)
	{
		globalCenter = offset;
	}
};