#pragma once

#include "src/Utils/Vector2.h"

/**
 * TransformComponent Component provides entities location
 * @param position Position from the bottom-left of the screen
 * @param scale X and Y scale
 * @param rotation Rotation
*/
struct TransformComponent
{
	Vector2 position;
	Vector2 scale;
	float rotation;

	explicit TransformComponent(const Vector2 position = Vector2(), const Vector2 scale = Vector2(), const float rotation = 0.0) :
		position(position),
		scale(scale),
		rotation(rotation)
	{}
};