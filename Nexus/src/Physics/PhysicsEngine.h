#pragma once

struct TransformComponent;
class RigidBodyComponent;
struct Vector2;

class PhysicsEngine
{
public:
	// Returns a new position by integrating the RigidBody's velocity and acceleration.
	static Vector2 Integrate(RigidBodyComponent& rigidBodyComponent, float dt);

	static void AddForce(RigidBodyComponent& rigidBodyComponent, const Vector2& force);

	static void ClearForces(RigidBodyComponent& rigidBodyComponent);

	// Drag Force = dragStrength * |v|^2 with direction opposite of velocity, where dragStrength = 1/2 * ρ * K * A
	static Vector2 GenerateDragForce(const RigidBodyComponent& rigidBodyComponent, const float dragStrength);

	// Friction Force = frictionStrength * |v| with direction opposite of velocity, where frictionStrength = μ
	static Vector2 GenerateFrictionForce(const RigidBodyComponent& rigidBodyComponent, const float frictionStrength);

	// Gravitational Force = G * ma * mb / |d|^2 with direction →d, where ma and mb is mass of two objects, d is distance and G is gravitational constant/strength
	static Vector2 GenerateGravitationalForce(const RigidBodyComponent& rigidBodyA, const RigidBodyComponent& rigidBodyB, const Vector2 distanceBMinusA, const float gravitationalStrength, float minDistance, float maxDistance);

	// Spring Force = -k * Δl, where k is spring constant(springForceStrength) and Δl is change in spring length
	static Vector2 GenerateSpringForce(const TransformComponent& transformComponent, Vector2 anchor, float restLength, float springForceStrength);
};