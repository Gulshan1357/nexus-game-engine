#include "stdafx.h"
#include "PhysicsEngine.h"

#include <algorithm>
#include <iostream>

#include "src/ECS/Entity.h"
#include "src/ECS/Coordinator.h"

#include "src/Components/RigidBodyComponent.h"
#include "src/Components/TransformComponent.h"

#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"
#include <src/Components/ColliderTypeComponent.h>

#include "src/Components/BoxColliderComponent.h"
#include "src/Components/CircleColliderComponent.h"
#include "src/Components/PolygonColliderComponent.h"

// TODO: Currently using the Implicit Euler Integration technique. Try Verlet Integration or RK4.
Vector2 PhysicsEngine::IntegrateLinear(RigidBodyComponent& rigidBodyComponent, float dt)
{
	// If the body have infinite mass, return 0 displacement
	if (rigidBodyComponent.IsStatic())
	{
		return {};
	}
	// Increment the original acceleration based on the applied forces and mass of the object (F = ma => a = F/m)
	rigidBodyComponent.acceleration = rigidBodyComponent.sumForces * rigidBodyComponent.inverseOfMass;

	// Calculating velocity by integrating acceleration
	rigidBodyComponent.velocity += rigidBodyComponent.acceleration * dt;

	// Calculating displacement by integrating velocity
	const Vector2 displacement = rigidBodyComponent.velocity * dt;
	ClearForces(rigidBodyComponent);
	// Logger::Warn("Forces resolved");
	return displacement;
}

float PhysicsEngine::IntegrateAngular(RigidBodyComponent& rigidBodyComponent, float dt)
{
	// If the body have infinite mass return 0 angular displacement
	if (rigidBodyComponent.IsStatic())
	{
		return {};
	}

	// Increment the angular acceleration based on the applied forces and angular mass(Moment of inertia or I) of the object (τ = Iα => α = τ / I)
	rigidBodyComponent.angularAcceleration += rigidBodyComponent.sumTorque * rigidBodyComponent.inverseOfAngularMass;

	// Calculating angular velocity by integrating angular acceleration
	rigidBodyComponent.angularVelocity += rigidBodyComponent.angularAcceleration * dt;

	// Calculating angular displacement by integrating angular velocity
	float angularDisplacement = rigidBodyComponent.angularVelocity * dt;

	ClearTorque(rigidBodyComponent);
	// Logger::Warn("Torque resolved");
	return angularDisplacement;
}

void PhysicsEngine::AddForce(RigidBodyComponent& rigidBodyComponent, const Vector2& force)
{
	// Logger::Warn("Adding force");
	rigidBodyComponent.sumForces += force;
}

void PhysicsEngine::ClearForces(RigidBodyComponent& rigidBodyComponent)
{
	rigidBodyComponent.sumForces = Vector2();
}

void PhysicsEngine::AddTorque(RigidBodyComponent& rigidBodyComponent, const float torque)
{
	// Logger::Warn("Adding Torque");
	rigidBodyComponent.sumTorque += torque;
}

void PhysicsEngine::ClearTorque(RigidBodyComponent& rigidBodyComponent)
{
	rigidBodyComponent.sumTorque = 0.0f;
}

float PhysicsEngine::CalculateMomentOfInertia(const Entity& entity)
{
	const auto& collider = entity.GetComponent<ColliderTypeComponent>();
	const auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

	switch (collider.type)
	{
		case ColliderType::Circle:
			if (entity.HasComponent<CircleColliderComponent>())
			{
				// For circle I = 1/2 * radius^2 * mass
				const auto& circleCollider = entity.GetComponent<CircleColliderComponent>();
				return 0.5f * rigidbody.mass * circleCollider.radius * circleCollider.radius;
			}
			break;
		case ColliderType::Box:
			if (entity.HasComponent<BoxColliderComponent>())
			{
				// For rectangle I = 1/12 * (width^2 + height^2) * mass
				const auto& boxCollider = entity.GetComponent<BoxColliderComponent>();
				return 0.083333f * rigidbody.mass * (boxCollider.width * boxCollider.width + boxCollider.height * boxCollider.height);
			}
			break;
		case ColliderType::Polygon:
			if (entity.HasComponent<PolygonColliderComponent>())
			{
				auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
				if (entity.HasTag("Debug"))
				{
					std::cout << "Inside MoI function, mass: " << rigidBody.mass << " I: " << rigidBody.angularMass << " inverse of I: " << rigidBody.inverseOfAngularMass << "\n";
				}

				const auto& polygonCollider = entity.GetComponent<PolygonColliderComponent>();
				// TODO: ...
				return 0.0f;
			}
	}
	Logger::Err("Couldn't calculate the Moment Of Inertia!");
	return 0.0f;
}

// Momentum P = mass * velocity
// Impulse is the change in momentum, J = ΔP = m*Δv
// Therefore, the change in velocity Δv = Impulse / mass
void PhysicsEngine::ApplyImpulse(RigidBodyComponent& rigidbody, const Vector2& impulse)
{
	if (rigidbody.IsStatic())
	{
		return;
	}
	rigidbody.velocity += impulse * rigidbody.inverseOfMass;
}

void PhysicsEngine::ApplyImpulse(RigidBodyComponent& rigidbody, const Vector2& impulse, const Vector2& r)
{
	if (rigidbody.IsStatic())
	{
		return;
	}
	rigidbody.velocity += impulse * rigidbody.inverseOfMass;
	rigidbody.angularVelocity += r.Cross(impulse) * rigidbody.inverseOfAngularMass;
}

Vector2 PhysicsEngine::GenerateDragForce(const RigidBodyComponent& rigidBodyComponent, const float dragStrength)
{
	const float velocitySquared = rigidBodyComponent.velocity.MagnitudeSquared();
	if (velocitySquared <= 0.0f)
	{
		return {}; // Returning Vector()
	}
	// Drag Direction is opposite to the velocity unit vector
	const Vector2 dragDirection = -rigidBodyComponent.velocity.UnitVector();

	// Drag magnitude = 1/2 * ρ * K * A * |v|^2, where ρ is density of fluid, K is (Drag Coefficient), A is cross-sectional area and v is Velocity
	// Since we are simulating for point objects A = 1 and since ρ and K are also constant in our case, the formula becomes
	// Drag magnitude = dragStrength * |v|^2, if combined constant dragStrength = 1/2 * ρ * K * A
	const float dragMagnitude = dragStrength * velocitySquared;
	// Logger::Warn("Physics engine drag: " + (dragDirection * dragMagnitude).ToString());

	return dragDirection * dragMagnitude;
}

Vector2 PhysicsEngine::GenerateFrictionForce(const RigidBodyComponent& rigidBodyComponent, const float frictionStrength)
{
	// Return zero vector if velocity is zero or nearly zero
	// if (rigidBodyComponent.velocity.MagnitudeSquared() <= std::numeric_limits<float>::epsilon())
	// {
	// 	return Vector2(0.0f, 0.0f);
	// }

	// Friction Direction is opposite to the velocity unit vector
	const Vector2 frictionDirection = -rigidBodyComponent.velocity.UnitVector();

	return frictionDirection * frictionStrength;
}

Vector2 PhysicsEngine::GenerateGravitationalForce(const RigidBodyComponent& rigidBodyA,
	const RigidBodyComponent& rigidBodyB, const Vector2 distanceBMinusA, const float gravitationalStrength, float minDistance, float maxDistance)
{
	float distanceSquared = distanceBMinusA.MagnitudeSquared();

	// Clamping the value of distanceSquared
	distanceSquared = std::clamp(distanceSquared, minDistance, maxDistance);

	const Vector2 attractionDirection = distanceBMinusA.UnitVector();

	const float attractionMagnitude = gravitationalStrength * (rigidBodyA.mass * rigidBodyB.mass) / distanceSquared;

	return attractionDirection * attractionMagnitude;
}

Vector2 PhysicsEngine::GenerateSpringForce(const TransformComponent& transformComponent, const Vector2 anchor, const float restLength, const float springForceStrength)
{
	// Distance between anchor and the object
	const Vector2 l = transformComponent.position - anchor;

	// Δl = current length - original length
	const float displacement = l.Magnitude() - restLength;

	// Spring Force = - k * Δl * direction vector = - springForceStrength * displacement * springDirection
	const Vector2 springDirection = l.UnitVector();
	const float springMagnitude = -springForceStrength * displacement;
	return springDirection * springMagnitude;
}

Vector2 PhysicsEngine::GenerateSpringForce(const TransformComponent& transformA, const TransformComponent& transformB,
	float restLength, float springForceStrength)
{
	// Distance between entities
	const Vector2 l = transformA.position - transformB.position;

	// Δl = current length - original length
	const float displacement = l.Magnitude() - restLength;

	// Spring Force = - k * Δl * direction vector = - springForceStrength * displacement * springDirection
	const Vector2 springDirection = l.UnitVector();
	const float springMagnitude = -springForceStrength * displacement;
	return springDirection * springMagnitude;
}

bool PhysicsEngine::IsAABBCollision(const double aX, const double aY, const double aW, const double aH, const double bX, const double bY, const double bW, const double bH)
{
	const bool isOverlappingX = aX < bX + bW && aX + aW > bX; // Are the x-axis projections of the bodies overlapping
	const bool isOverlappingY = aY < bY + bH && aY + aH > bY; // Are the y-axis projections of the bodies overlapping
	return 	isOverlappingX && isOverlappingY;
}

bool PhysicsEngine::IsSATCollision(const std::vector<Vector2>& verticesA, const std::vector<Vector2>& verticesB, Vector2& outStartContactPoint, Vector2& outEndContactPoint, Vector2& outCollisionNormalAxis, float& outMinPenetration)
{
	// The goal is to find a separating axis. If there exist an axis where the projection of the polygons are separated, then the polygons are not colliding.

	// Helper function to calculate the minimum separation between two sets of vertices.
	auto findMinSeparation = [](const std::vector<Vector2>& primaryVertices, const std::vector<Vector2>& secondaryVertices, Vector2& outEdgeAxis, Vector2& outContactPoint)
		{
			float maxSeparation = std::numeric_limits<float>::lowest();

			// Loop through all edges of the primary polygon.
			for (size_t i = 0; i < primaryVertices.size(); ++i)
			{
				Vector2 vertexA = primaryVertices[i];
				Vector2 edgeA = primaryVertices[(i + 1) % primaryVertices.size()] - vertexA;
				Vector2 edgeNormal = edgeA.Normal();  // Get perpendicular (normal) to the edge

				// Find the minimum projection distance for the secondary polygon onto this edge normal.
				float minProjection = std::numeric_limits<float>::max();
				Vector2 closestVertex;

				// Loop through all the vertices of the secondary polygon.
				for (auto vertexB : secondaryVertices)
				{
					// Calculate the projection of the vector from the vertexA to the vertexB onto the edge normal.
					float projection = (vertexB - vertexA).Dot(edgeNormal);
					if (projection < minProjection)
					{
						minProjection = projection;
						closestVertex = vertexB; // Track the vertex causing the minimum separation.
					}
				}

				// Update the maximum separation along the current edge normal.
				if (minProjection > maxSeparation)
				{
					maxSeparation = minProjection;
					outEdgeAxis = primaryVertices[(i + 1) % primaryVertices.size()] - vertexA;
					outContactPoint = closestVertex;
				}
			}
			return maxSeparation;
		};

	Vector2 aAxis, bAxis;
	Vector2 aContactPoint, bContactPoint;

	const float abSeparation = findMinSeparation(verticesA, verticesB, aAxis, aContactPoint);
	if (abSeparation >= 0)
	{
		return false; // Separating axis found, no collision.
	}

	const float baSeparation = findMinSeparation(verticesB, verticesA, bAxis, bContactPoint);
	if (baSeparation >= 0)
	{
		return false; // Separating axis found, no collision.
	}

	// Determine the collision normal and penetration based on the smallest separation.
	if (abSeparation > baSeparation)
	{
		outMinPenetration = -abSeparation;
		outCollisionNormalAxis = aAxis.Normal();
		outStartContactPoint = aContactPoint;
		outEndContactPoint = aContactPoint + outCollisionNormalAxis * outMinPenetration;
	}
	else
	{
		outMinPenetration = -baSeparation;
		outCollisionNormalAxis = -bAxis.Normal();
		outStartContactPoint = bContactPoint - outCollisionNormalAxis * outMinPenetration;
		outEndContactPoint = bContactPoint;
	}

	return true; // No separating axis found, collision occurs
}

void PhysicsEngine::ResolvePenetration(const float depth, const Vector2 collisionNormal, const RigidBodyComponent& aRigidbody, const RigidBodyComponent& bRigidbody, TransformComponent& aTransform, TransformComponent& bTransform)
{
	if (aRigidbody.IsStatic() && bRigidbody.IsStatic())
	{
		return;
	}

	const float displacementA = depth / (aRigidbody.inverseOfMass + bRigidbody.inverseOfMass) * aRigidbody.inverseOfMass;
	const float displacementB = depth / (aRigidbody.inverseOfMass + bRigidbody.inverseOfMass) * bRigidbody.inverseOfMass;

	aTransform.position -= collisionNormal * displacementA;
	bTransform.position += collisionNormal * displacementB;
}

// (1) New Relative velocity along the collision normal =  - elasticity(E) (Relative velocity along collision normal)
// Since, Impulse = change in momentum, where momentum is the product of mass and velocity
// (2) => New Velocity = old velocity + (Impulse along collision normal / Mass)
// (3) Also, New Relative velocity along collision normal = (New velocity of A - New Velocity of B) along collision normal
// (From 2 & 3) New relative velocity = (Va - Vb) + (Impulse along collision normal/Ma + Impulse along collision normal/Mb)
// After substituting (1), Impulse (linear) = - (1 + E) * (vRelative . collisionNormal) / (1/Ma + 1/Mb)
// For linear + angular Impulse: https://medium.com/@www.seymour/coding-a-2d-physics-engine-from-scratch-and-using-it-to-simulate-a-pendulum-clock-964b4ac2107a
void PhysicsEngine::ResolveCollision(const Vector2 startContactPoint, const Vector2 endContactPoint, const float depth, const Vector2 collisionNormal, RigidBodyComponent& aRigidbody, RigidBodyComponent& bRigidbody, TransformComponent& aTransform, TransformComponent& bTransform)
{
	// Apply position correction using the projection method
	ResolvePenetration(depth, collisionNormal, aRigidbody, bRigidbody, aTransform, bTransform);

	// Note: Regarding cross product, since we are working in a 2d space, the result of 2D cross product will be
	// the scalar magnitude of the z-component of the result perpendicular (to the screen) vector. 

	// Resultant elasticity/friction is the minimum coefficient of restitution among the two rigid-bodies.
	const float e = std::min(aRigidbody.restitution, bRigidbody.restitution);
	const float f = std::min(aRigidbody.friction, bRigidbody.friction);

	const Vector2 rA = endContactPoint - aTransform.position; // Distance from the center of 'A' to the point of contact
	const Vector2 rB = startContactPoint - bTransform.position; // Distance from the center of 'B' to the point of contact

	// Calculating the combined linear velocity and angular velocity (ω X r). The X denotes cross product. Since we are working in 2d the ω just a magnitude with positive value means towards the player and negative means inside the screen. For cross product we can consider the ω magnitude as a z- axis component of the hypothetical ω vector with x and y components as 0. 
	const Vector2 vA = aRigidbody.velocity + Vector2(-(aRigidbody.angularVelocity * rA.y), (aRigidbody.angularVelocity * rA.y)); // Combined linear and angular velocity of 'A'
	const Vector2 vB = bRigidbody.velocity + Vector2(-(bRigidbody.angularVelocity * rB.y), (bRigidbody.angularVelocity * rB.y)); // Combined linear and angular velocity of 'B'

	// Relative velocity between the two objects
	const Vector2 vRel = vA - vB;

	// Calculating Impulse along the normal
	const float vRelDotNormal = vRel.Dot(collisionNormal); // Relative velocity along the collision normal vector
	const Vector2 impulseNormalDirection = collisionNormal; // Impulse direction is the same as collision normal
	const float impulseNormalMagnitude =
		-(1 + e) * vRelDotNormal / (
			(aRigidbody.inverseOfMass + bRigidbody.inverseOfMass) +
			(rA.Cross(collisionNormal) * rA.Cross(collisionNormal) * aRigidbody.inverseOfAngularMass) +
			(rB.Cross(collisionNormal) * rB.Cross(collisionNormal) * bRigidbody.inverseOfAngularMass)
			); // Magnitude of linear + angular impulse
	const Vector2 impulseNormal = impulseNormalDirection * impulseNormalMagnitude;

	// Calculating Impulse along the tangent (Friction Impulse)
	const Vector2 tangent = collisionNormal.Normal(); // Perpendicular to collisionNormal (direction of friction)
	const float vRelDotTangent = vRel.Dot(tangent); // Relative velocity along the tangent
	const Vector2 impulseTangentDirection = tangent; // Friction Impulse direction is the same as tangent
	const float impulseTangentMagnitude =
		f * -(1 + e) * vRelDotTangent / (
			(aRigidbody.inverseOfMass + bRigidbody.inverseOfMass) +
			(rA.Cross(tangent) * rA.Cross(tangent) * aRigidbody.inverseOfAngularMass) +
			(rB.Cross(tangent) * rB.Cross(tangent) * bRigidbody.inverseOfAngularMass)
			); // Magnitude of linear + angular friction impulse
	const Vector2 impulseTangent = impulseTangentDirection * impulseTangentMagnitude;

	Vector2 totalImpulse = impulseNormal + impulseTangent;

	aRigidbody.ApplyImpulse(totalImpulse, rA);
	bRigidbody.ApplyImpulse(-totalImpulse, rB);
}
