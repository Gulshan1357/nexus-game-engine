#pragma once

#include "../Utils/Vector2.h"

struct TransformComponent
{
	Vector2 position;
	Vector2 scale;
	float rotation;

	TransformComponent(Vector2 position = Vector2(), Vector2 scale = Vector2(), float rotation = 0.0) :
		position(position),
		scale(scale),
		rotation(rotation)
	{}
};