#include "stdafx.h"
#include "PhysicsEngine.h"

#include <iostream>

#include "src/Components/RigidBodyComponent.h"

#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"

// TODO: Currently using the Implicit Euler Integration technique. Try Verlet Integration or RK4.
Vector2 PhysicsEngine::Integrate(RigidBodyComponent& rigidBodyComponent, float dt)
{
	// Increment the original acceleration based on the applied forces and mass of the object (F = ma => a = F/m)
	rigidBodyComponent.acceleration = rigidBodyComponent.m_sumForces * rigidBodyComponent.m_inverseOfMass;

	// Calculating velocity by integrating acceleration
	rigidBodyComponent.velocity += rigidBodyComponent.acceleration * dt;

	// Calculating position by integrating velocity
	const Vector2 position = rigidBodyComponent.velocity * dt;
	ClearForces(rigidBodyComponent);
	// Logger::Warn("Forces resolved");
	return position;
}

void PhysicsEngine::AddForce(RigidBodyComponent& rigidBodyComponent, const Vector2& force)
{
	// Logger::Warn("Adding force");
	rigidBodyComponent.m_sumForces += force;
}

void PhysicsEngine::ClearForces(RigidBodyComponent& rigidBodyComponent)
{
	rigidBodyComponent.m_sumForces = Vector2();
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
