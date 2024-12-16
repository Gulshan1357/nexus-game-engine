#pragma once

class RigidBodyComponent;
struct Vector2;

class PhysicsEngine
{
public:
	// Returns a new position by integrating the RigidBody's velocity and acceleration. It modifies the velocity.
	static Vector2 Integrate(RigidBodyComponent& rigidBodyComponent, float dt);
};

