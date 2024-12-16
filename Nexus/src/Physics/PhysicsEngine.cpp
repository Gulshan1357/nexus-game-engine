#include "stdafx.h"
#include "PhysicsEngine.h"

#include "src/Components/RigidBodyComponent.h"

#include "src/Utils/Vector2.h"

// TODO: Currently using the Implicit Euler Integration technique. Try Verlet Integration or RK4.
Vector2 PhysicsEngine::Integrate(RigidBodyComponent& rigidBodyComponent, float dt)
{
	rigidBodyComponent.velocity += rigidBodyComponent.acceleration * dt;
	return rigidBodyComponent.velocity * dt;
}

