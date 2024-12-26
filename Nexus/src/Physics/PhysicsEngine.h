#pragma once
#include <vector>


struct TransformComponent;
class RigidBodyComponent;
struct Vector2;
class Entity;

class PhysicsEngine
{
public:
	//------------------------------------------------------------------------
	// Integrating Force and Torque
	//------------------------------------------------------------------------

	// Returns a new position by integrating the RigidBody's velocity and acceleration.
	static Vector2 IntegrateLinear(RigidBodyComponent& rigidBodyComponent, float dt);

	// Returns a new rotation by integrating the RigidBody's angular velocity and angular acceleration.
	static float IntegrateAngular(RigidBodyComponent& rigidBodyComponent, float dt);

	static void AddForce(RigidBodyComponent& rigidBodyComponent, const Vector2& force);

	static void ClearForces(RigidBodyComponent& rigidBodyComponent);

	static void AddTorque(RigidBodyComponent& rigidBodyComponent, const float torque);

	static void ClearTorque(RigidBodyComponent& rigidBodyComponent);

	static float CalculateMomentOfInertia(const Entity& entity);


	//------------------------------------------------------------------------
	// Different Forces
	//------------------------------------------------------------------------

	// Drag Force = dragStrength * |v|^2 with direction opposite of velocity, where dragStrength = 1/2 * ρ * K * A
	static Vector2 GenerateDragForce(const RigidBodyComponent& rigidBodyComponent, const float dragStrength);

	// Friction Force = frictionStrength * |v| with direction opposite of velocity, where frictionStrength = μ
	static Vector2 GenerateFrictionForce(const RigidBodyComponent& rigidBodyComponent, const float frictionStrength);

	// Gravitational Force = G * ma * mb / |d|^2 with direction →d, where ma and mb is mass of two objects, d is distance and G is gravitational constant/strength
	static Vector2 GenerateGravitationalForce(const RigidBodyComponent& rigidBodyA, const RigidBodyComponent& rigidBodyB, const Vector2 distanceBMinusA, const float gravitationalStrength, float minDistance, float maxDistance);

	// Spring Force between an entity and anchor = -k * Δl, where k is spring constant(springForceStrength) and Δl is change in spring length
	static Vector2 GenerateSpringForce(const TransformComponent& transformComponent, Vector2 anchor, float restLength, float springForceStrength);

	// Spring Force between two entities = -k * Δl, where k is spring constant(springForceStrength) and Δl is change in spring length
	static Vector2 GenerateSpringForce(const TransformComponent& transformA, const TransformComponent& transformB, float restLength, float springForceStrength);


	//------------------------------------------------------------------------
	// Collision Detection
	//------------------------------------------------------------------------

	// Axis-Aligned Bounding Box for Collision Detection. Input is the bottom left point of each rectangle along with width and height. Returns true if the two rectangles are colliding.
	static bool IsAABBCollision(const double aX, const double aY, const double aW, const double aH, const double bX, const double bY, const double bW, const double bH);

	// Separating Axis Theorem for Collision Detection between two convex polygon. Input are the list of vertices for each polygon. Returns true if the two polygons are colliding. It also update the values of minimum penetration and collision normal which can be used for collision resolution
	static bool IsSATCollision(const std::vector<Vector2>& verticesA, const std::vector<Vector2>& verticesB, float& minPenetration, Vector2& collisionNormal);
};