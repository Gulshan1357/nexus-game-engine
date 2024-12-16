#pragma once

class RigidBodyComponent;
struct Vector2;

class PhysicsEngine
{
public:
	// Returns a new position by integrating the RigidBody's velocity and acceleration.
	static Vector2 Integrate(RigidBodyComponent& rigidBodyComponent, float dt);

	static void AddForce(RigidBodyComponent& rigidBodyComponent, const Vector2& force);

	static void ClearForces(RigidBodyComponent& rigidBodyComponent);
};

