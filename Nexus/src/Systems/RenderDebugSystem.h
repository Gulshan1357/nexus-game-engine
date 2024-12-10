#pragma once

#include "../App/app.h"

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/BoxColliderComponent.h"

class RenderDebugSystem : public System
{
public:
	RenderDebugSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update()
	{
		for (auto entity : GetSystemEntities())
		{
			// Printing a red rectangle around box collider

			// Retrieve the transform and collider components of the entity
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& collider = entity.GetComponent<BoxColliderComponent>();

			// To reduce the amount of calculations, calculating two points the bottom-left and collider's center.
			// The rest of the points are calculated based on these two points and collider's width and height.

			float colliderCenterX = transform.position.x + collider.offset.x;
			float colliderCenterY = transform.position.y + collider.offset.y;

			float halfWidth = static_cast<float>(collider.width) / 2.0f;
			float halfHeight = static_cast<float>(collider.height) / 2.0f;

			float bottomLeftX = colliderCenterX - halfWidth;
			float bottomLeftY = colliderCenterY - halfHeight;

			// Draw a rectangle around the collider
			App::DrawLine(bottomLeftX, bottomLeftY, colliderCenterX + halfWidth, bottomLeftY, 1.0f, 0.0f, 0.0f);
			App::DrawLine(colliderCenterX + halfWidth, bottomLeftY, colliderCenterX + halfWidth, colliderCenterY + halfHeight, 1.0f, 0.0f, 0.0f);
			App::DrawLine(colliderCenterX + halfWidth, colliderCenterY + halfHeight, bottomLeftX, colliderCenterY + halfHeight, 1.0f, 0.0f, 0.0f);
			App::DrawLine(bottomLeftX, colliderCenterY + halfHeight, bottomLeftX, bottomLeftY, 1.0f, 0.0f, 0.0f);
		}
	}
};