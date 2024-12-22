#pragma once

#include "src/ECS/System.h"

#include "src/Components/ColliderTypeComponent.h"
#include "src/Components/RigidbodyComponent.h"

class PhysicsSystem : public System
{
public:
	PhysicsSystem()
	{
		RequireComponent<RigidBodyComponent>();
	}

	void InitializeEntityPhysics() const
	{
		Logger::Log("Initializing Entity Physics. Calculating inverse mass, angular mass etc.");
		// Calculate inverse mass, angular mass and inverse angular mass of entities with RigidBody
		for (auto entity : GetSystemEntities())
		{
			auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
			if (rigidBody.bUsePhysics)
			{
				rigidBody.inverseOfMass = (rigidBody.mass != 0.0f) ? 1.0f / rigidBody.mass : 0.0f;

				if (entity.HasComponent<ColliderTypeComponent>())
				{
					rigidBody.angularMass = PhysicsEngine::CalculateMomentOfInertia(entity);
					rigidBody.inverseOfAngularMass = (rigidBody.angularMass != 0.0f) ? 1.0f / rigidBody.angularMass : 0.0f;
				}
			}
		}
	}
};