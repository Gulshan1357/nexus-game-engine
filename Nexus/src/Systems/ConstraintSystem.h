#pragma once

#include "src/ECS/System.h"

#include "src/Components/ConstraintComponent.h"

class ConstraintSystem : public System
{
public:
	ConstraintSystem()
	{
		RequireComponent<ConstraintComponent>();
	}

	void Update(const float deltaTime)
	{
		// TODO: resolve constrains for non-kinematic bodies
		// for (auto entity : GetSystemEntities())
		// {
		// 	auto& transform = entity.GetComponent<TransformComponent>();
		// 	auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
		//
		// 	if (!rigidBody.isKinematic)
		// 	{
		// 	}
		// }
	}
};
