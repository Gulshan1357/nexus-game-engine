#pragma once

#include "src/Physics/PhysicsEngine.h"
#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"

/**
 * RigidBody Component is the physics components that provides simulations like velocity and acceleration
 * @param velocity (Vector2): Add this Vector to TransformComponent's position vector every delta time
 * @param acceleration (Vector2): Initial acceleration of the RigidBody. If bUsePhysics = true then the additional forces will change this value.
 * @param bUsePhysics (Bool): Default value is false. If set to true then it uses PhysicsEngine for stimulating forces and mass to drive acceleration. If set to false then the original acceleration remains the same.
 * @param mass (Float): Default value is set to 1.
*/
class RigidBodyComponent
{
public:
	friend class PhysicsEngine; // So that PhysicsEngine can access m_sumForces

	Vector2 velocity;
	Vector2 acceleration;

	bool bUsePhysics;
	float mass;

	explicit RigidBodyComponent(Vector2 velocity = Vector2(), Vector2 acceleration = Vector2(), bool bUsesPhysics = false, float mass = 1.0f) :
		velocity(velocity), acceleration(acceleration), bUsePhysics(bUsesPhysics), mass(mass)
	{
		// Logger::Log("RigidBody constructor called!");

		// Calculating m_inverseOfMass once when the RigidBodyComponent Constructor is called
		if (mass != 0.0f) m_inverseOfMass = 1.0f / mass;
		else m_inverseOfMass = 0.0f;
	}

	// ~RigidBodyComponent()
	// {
	// 	Logger::Log("RigidBody destructor called!");
	// }

	// Public wrapper to call PhysicsEngine's AddForce. While adding a force make sure to multiply the values by Physics::PIXEL_PER_METER
	void AddForce(const Vector2& force)
	{
		PhysicsEngine::AddForce(*this, force);
	}

private:
	Vector2 m_sumForces; // Tracks accumulated forces.
	float m_inverseOfMass; // Keeping track of 1/Mass to avoid doing this costly calculation multiple times
};