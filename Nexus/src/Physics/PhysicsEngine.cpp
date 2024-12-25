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
	// Increment the original acceleration based on the applied forces and mass of the object (F = ma => a = F/m)
	rigidBodyComponent.acceleration = rigidBodyComponent.sumForces * rigidBodyComponent.inverseOfMass;

	// Calculating velocity by integrating acceleration
	rigidBodyComponent.velocity += rigidBodyComponent.acceleration * dt;

	// Calculating position by integrating velocity
	const Vector2 position = rigidBodyComponent.velocity * dt;
	ClearForces(rigidBodyComponent);
	// Logger::Warn("Forces resolved");
	return position;
}

float PhysicsEngine::IntegrateAngular(RigidBodyComponent& rigidBodyComponent, float dt)
{
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
	return (
		aX < bX + bW &&
		aX + aW > bX &&
		aY < bY + bH &&
		aY + aH > bY
		);
}

bool PhysicsEngine::IsSATCollision(const std::vector<Vector2>& verticesA, const std::vector<Vector2>& verticesB)
{
	// The goal is to find a separating axis. If found then the entities are not colliding.

	// Generate all potential separating axes from both polygons by calculating the perpendicular using Vector2.Normal()
	std::vector<Vector2> axes;

	for (size_t i = 0; i < verticesA.size(); ++i)
	{
		Vector2 edge = verticesA[(i + 1) % verticesA.size()] - verticesA[i];
		axes.push_back(edge.Normal().Normalize());
	}

	for (size_t i = 0; i < verticesB.size(); ++i)
	{
		Vector2 edge = verticesB[(i + 1) % verticesB.size()] - verticesB[i];
		axes.push_back(edge.Normal().Normalize());
	}

	// Lambda function to check if there is an overlap. This function will be called for each axis.
	auto checkAxisOverlap = [](const Vector2& axis, const std::vector<Vector2>& verticesA, const std::vector<Vector2>& verticesB)
		{
			float minA = FLT_MAX, maxA = -FLT_MAX;
			float minB = FLT_MAX, maxB = -FLT_MAX;

			// Loop through all the vertices to find the minimum and maximum projection by each polygon's vertices on the axis.

			for (const auto& vertex : verticesA)
			{
				float projection = axis.Dot(vertex);
				minA = std::min(minA, projection);
				maxA = std::max(maxA, projection);
			}

			for (const auto& vertex : verticesB)
			{
				float projection = axis.Dot(vertex);
				minB = std::min(minB, projection);
				maxB = std::max(maxB, projection);
			}

			// The axis has no overlap. This means the two convex polygons are not colliding
			if (maxA < minB || maxB < minA)
			{
				return false;
			}

			return true;
		};

	// Calling the lambda function for each axis to find the separating axes
	for (const auto& axis : axes)
	{
		if (!checkAxisOverlap(axis, verticesA, verticesB))
		{
			return false; // Separating axis found, no collision
		}
	}
	return true; // No separating axis found, collision occurs
}
