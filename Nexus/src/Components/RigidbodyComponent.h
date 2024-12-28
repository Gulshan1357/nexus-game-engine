#pragma once

#include "src/Physics/PhysicsEngine.h"
#include "src/Utils/Vector2.h"

/**
 * RigidBody Component is the physics components that provides simulations like velocity and acceleration
 * @param velocity (Vector2): Add this Vector to TransformComponent's position vector every delta time
 * @param acceleration (Vector2): Initial acceleration of the RigidBody. If bUsePhysics = true then the additional forces will change this value.
 * @param bUsePhysics (Bool): Default value is false. If set to true then it uses PhysicsEngine for stimulating forces and mass to drive acceleration. If set to false it can still move but the original acceleration remains the same. Toggling this value at runtime won't automatically calculate these values.
 * @param mass (Float): Default value is set to 1. If mass is 0 then object is static and it won't move (like floor)
 * @param angularVelocity (Float): Default value is set to 0.
 * @param angularAcceleration (Float): Default value is set to 0.
 * @param restitution (Float value between 0.0 - 1.0): Elasticity. Default is 1.0f
*/
class RigidBodyComponent
{
public:
	// For linear motion
	Vector2 velocity;
	Vector2 acceleration;

	bool bUsePhysics;

	float mass;

	// For Angular motion
	float angularVelocity;
	float angularAcceleration;

	// Coefficient of restitution (elasticity)
	float restitution;

	// Initialized later by Physics System
	float angularMass = 0.0f;			// Moment of Inertia (angularMass)
	Vector2 sumForces = Vector2();		// Tracks accumulated linear forces.
	float sumTorque = 0.0f;				// Tracks accumulated angular forces (Torque).
	float inverseOfMass = 1.0f;			// Keeping track of 1/Mass to avoid doing this costly calculation multiple times
	float inverseOfAngularMass = 0.0f;	// Keeping track of 1/AngularMass to avoid doing this costly calculation multiple times

	explicit RigidBodyComponent(
		const Vector2 velocity = Vector2(),
		const Vector2 acceleration = Vector2(),
		const bool bUsesPhysics = false,
		const float mass = 1.0f,
		const float angularVelocity = 0.0f,
		const float angularAcceleration = 0.0f,
		const float restitution = 1.0f
	) :
		velocity(velocity),
		acceleration(acceleration),
		bUsePhysics(bUsesPhysics),
		mass(mass),
		angularVelocity(angularVelocity),
		angularAcceleration(angularAcceleration),
		restitution(restitution)
	{}


	//------------------------------------------------------------------------
	// Helper functions
	//------------------------------------------------------------------------

	// Checks if inverse of mass is zero. If return is true this means the body have infinite mass
	[[nodiscard]] bool IsStatic() const
	{
		return fabs(inverseOfMass - 0.0f) < FLT_EPSILON;
	}

	// Add Force
	void AddForce(const Vector2& force)
	{
		PhysicsEngine::AddForce(*this, force);
	}

	// Add Torque
	void AddTorque(const float torque)
	{
		PhysicsEngine::AddTorque(*this, torque);
	}

	// Apply impulse to the RigidBody. Change in Velocity, Δv = Impulse / mass
	void ApplyImpulse(const Vector2 impulse)
	{
		PhysicsEngine::ApplyImpulse(*this, impulse);
	}
};