#pragma once

#include "src/Physics/PhysicsEngine.h"
#include "src/Utils/Vector2.h"

/**
 * RigidBody Component is the physics components that provides simulations like velocity and acceleration
 * @param velocity (Vector2): Add this Vector to TransformComponent's position vector every delta time
 * @param acceleration (Vector2): Initial acceleration of the RigidBody. If isKinematic = false then the additional forces will change this value.
 * @param isKinematic (Bool): Default value is true. If set to true then forces, collisions or joints won't affect the rigidbody. Toggling this value at runtime won't automatically calculate these values, you alos have to call the Physics::InitializeEntityPhysics()
 * @param mass (Float): Default value is set to 1. If mass is 0 then object is static and it won't move (like floor)
 * @param angularVelocity (Float): Default value is set to 0.
 * @param angularAcceleration (Float): Default value is set to 0.
 * @param restitution (Float value between 0.0 - 1.0): Elasticity. Default is 1.0f
 * @param friction (Float value between 0.0 - 1.0): Coefficient of friction. Default is 0.7f
*/
class RigidBodyComponent
{
public:
	// For linear motion
	Vector2 velocity;
	Vector2 acceleration;

	bool isKinematic;

	float mass;

	// For Angular motion
	float angularVelocity;
	float angularAcceleration;

	// Coefficient of restitution (elasticity)
	float restitution;

	// Coefficient of friction
	float friction;

	// Initialized later by Physics System
	float angularMass = 0.0f;			// Moment of Inertia (angularMass)
	Vector2 sumForces = Vector2();		// Tracks accumulated linear forces.
	float sumTorque = 0.0f;				// Tracks accumulated angular forces (Torque).
	float inverseOfMass = 1.0f;			// Keeping track of 1/Mass to avoid doing this costly calculation multiple times
	float inverseOfAngularMass = 0.0f;	// Or inverse of Moment of Inertia. Keeping track of 1/AngularMass  to avoid doing this costly calculation multiple times

	explicit RigidBodyComponent(
		const Vector2 velocity = Vector2(),
		const Vector2 acceleration = Vector2(),
		bool isKinematic = true,
		const float mass = 1.0f,
		const float angularVelocity = 0.0f,
		const float angularAcceleration = 0.0f,
		const float restitution = 1.0f,
		const float friction = 0.7f
	) :
		velocity(velocity),
		acceleration(acceleration),
		isKinematic(isKinematic),
		mass(mass),
		angularVelocity(angularVelocity),
		angularAcceleration(angularAcceleration),
		restitution(restitution),
		friction(friction)
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

	// Apply linear impulse to the RigidBody's center of mass. Change in Velocity, Δv = Impulse / mass
	void ApplyImpulseLinear(const Vector2 impulse)
	{
		PhysicsEngine::ApplyImpulseLinear(*this, impulse);
	}

	// Apply angular impulse to the RigidBody's center of mass.
	void ApplyImpulseAngular(const float impulse)
	{
		PhysicsEngine::ApplyImpulseAngular(*this, impulse);
	}

	// Apply linear + angular impulse to the point at a certain distance from RigidBody's center of mass.
	void ApplyImpulseAngular(const Vector2 impulse, const Vector2& distanceFromCenter)
	{
		PhysicsEngine::ApplyImpulseAtPoint(*this, impulse, distanceFromCenter);
	}
};