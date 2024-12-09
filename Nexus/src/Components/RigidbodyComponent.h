#pragma once

#include "src/Utils/Vector2.h"

struct RigidBodyComponent
{
	Vector2 velocity;

	RigidBodyComponent(Vector2 velocity = Vector2()) : velocity(velocity) {}
};