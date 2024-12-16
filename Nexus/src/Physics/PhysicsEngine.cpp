#include "stdafx.h"
#include "PhysicsEngine.h"

#include "src/Components/RigidBodyComponent.h"

#include "src/Utils/Vector2.h"

// TODO: Currently using the Implicit Euler Integration technique. Try Verlet Integration or RK4.
Vector2 PhysicsEngine::Integrate(RigidBodyComponent& rigidBodyComponent, float dt)
{
	// Increment the original acceleration based on the applied forces and mass of the object (F = ma => a = F/m)
	rigidBodyComponent.acceleration += rigidBodyComponent.m_sumForces / rigidBodyComponent.mass;

	// Calculating velocity by integrating acceleration
	rigidBodyComponent.velocity += rigidBodyComponent.acceleration * dt;

	// Calculating position by integrating velocity
	const Vector2 position = rigidBodyComponent.velocity * dt;

	ClearForces(rigidBodyComponent);

	return position;
}

void PhysicsEngine::AddForce(RigidBodyComponent& rigidBodyComponent, const Vector2& force)
{
	rigidBodyComponent.m_sumForces += force;
}

void PhysicsEngine::ClearForces(RigidBodyComponent& rigidBodyComponent)
{
	rigidBodyComponent.m_sumForces = Vector2();
}

