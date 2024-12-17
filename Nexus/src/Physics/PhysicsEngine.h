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

	// Drag Force = k * |v|^2 with direction opposite of velocity. k = 1/2 * ρ * K * A
	static Vector2 GenerateDragForce(const RigidBodyComponent& rigidBodyComponent, float k);
};