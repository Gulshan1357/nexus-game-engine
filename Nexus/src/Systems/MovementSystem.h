#pragma once

#include "src/ECS/Entity.h"
#include "src/ECS/System.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/RigidBodyComponent.h"

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void Update(const float deltaTime) const
	{
		for (auto entity : GetSystemEntities())
		{
			TransformComponent& transform = entity.GetComponent<TransformComponent>();
			const RigidBodyComponent& rigidbody = entity.GetComponent<RigidBodyComponent>();

			transform.position.x += rigidbody.velocity.x * deltaTime;
			transform.position.y += rigidbody.velocity.y * deltaTime;

			// Send event to stop animation (player->animationComp->bIsActive) when velocity is zero
		}
	}
};