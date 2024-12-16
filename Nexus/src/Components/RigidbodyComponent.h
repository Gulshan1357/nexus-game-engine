#pragma once

#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"

/**
 * RigidBody Component is the physics components that provides simulations like velocity and acceleration
 * @param velocity : Add this Vector to TransformComponent's position vector every delta time
 * @param acceleration : Self-explanatory
 * @param bUsePhysics : If set to true then it uses PhysicsEngine for physics simulation.
 * @param mass : Self-explanatory
*/
class RigidBodyComponent
{
public:
	Vector2 velocity;
	Vector2 acceleration;

	bool bUsePhysics;
	float mass;

	explicit RigidBodyComponent(Vector2 velocity = Vector2(), Vector2 acceleration = Vector2(), bool bUsesPhysics = false, float mass = 1.0f) :
		velocity(velocity), acceleration(acceleration), bUsePhysics(bUsesPhysics), mass(mass)
	{
		// Logger::Log("RigidBody constructor called!");
	}

	// ~RigidBodyComponent()
	// {
	// 	Logger::Log("RigidBody destructor called!");
	// }
};