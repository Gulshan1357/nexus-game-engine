#pragma once

#include "src/Utils/Vector2.h"

struct BoxColliderComponent
{
	float width;
	float height;
	Vector2 offset;

	explicit BoxColliderComponent(const float width = 0, const float height = 0, const Vector2 offset = Vector2())
		: width(width), height(height), offset(offset)
	{}
};