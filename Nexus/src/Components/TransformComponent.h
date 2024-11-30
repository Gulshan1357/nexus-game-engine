#pragma once

#include <src/Utils/Logger.h>

struct TransformComponent
{
	float position;
	float scale;
	float rotation;

	TransformComponent(float position = 0.f, float scale = 0.f, float rotation = 0.0f) :
		position(position),
		scale(scale),
		rotation(rotation)
	{
		Logger::Log("Added TransformComponent");
	}
};