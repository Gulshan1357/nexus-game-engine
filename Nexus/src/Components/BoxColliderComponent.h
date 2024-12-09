#pragma once

#include "src/Utils/Vector2.h"

struct BoxColliderComponent
{
	int width;
	int height;
	Vector2 offset;

	BoxColliderComponent(int width = 0, int height = 0, Vector2 offset = Vector2())
		: width(width), height(height), offset(offset)
	{}
};