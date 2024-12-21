#pragma once

#include "src/Utils/Vector2.h"

/**
 * BoxColliderComponent Component provides information for Collision System
 * @param width (float)  of the BoxCollider
 * @param height (float) of the BoxCollider
 * @param offset (Vector2)
*/
struct BoxColliderComponent
{
	float width;
	float height;
	Vector2 offset;

	explicit BoxColliderComponent(const float width = 0, const float height = 0, const Vector2 offset = Vector2())
		: width(width), height(height), offset(offset)
	{}
};