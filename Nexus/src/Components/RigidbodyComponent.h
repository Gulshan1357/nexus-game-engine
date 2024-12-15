#pragma once

#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"

/**
 * RigidBody Component is the physics components that provides simulations like velocity and acceleration
 * @param velocity Add this Vector to TransformComponent's position vector every delta time
 * @param acceleration Self-explanatory
 * @param mass Self-explanatory
*/
struct RigidBodyComponent
{
	Vector2 velocity;
	Vector2 acceleration;

	float mass;

	explicit RigidBodyComponent(Vector2 velocity = Vector2(), Vector2 acceleration = Vector2(), float mass = 1.0f) :
		velocity(velocity), acceleration(acceleration), mass(mass)
	{
		Logger::Log("RigidBody constructor called!");
	}
	
	// ~RigidBodyComponent()
	// {
	// 	Logger::Log("RigidBody destructor called!");
	// }
};