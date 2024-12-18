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

	void Update() const
	{
		for (auto entity : GetSystemEntities())
		{
			// Printing a red rectangle around box collider

			// Retrieve the transform and collider components of the entity
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& collider = entity.GetComponent<BoxColliderComponent>();

			// To reduce the amount of calculations, calculating two points the bottom-left and collider's center.
			// The rest of the points are calculated based on these two points and collider's width and height.

			const float colliderCenterX = transform.position.x + collider.offset.x;
			const float colliderCenterY = transform.position.y + collider.offset.y;

			const float halfWidth = static_cast<float>(collider.width) / 2.0f;
			const float halfHeight = static_cast<float>(collider.height) / 2.0f;

			const float bottomLeftX = colliderCenterX - halfWidth;
			const float bottomLeftY = colliderCenterY - halfHeight;

			// Draw a rectangle around the collider
			App::DrawLine(bottomLeftX, bottomLeftY, colliderCenterX + halfWidth, bottomLeftY, 1.0f, 0.0f, 0.0f);
			App::DrawLine(colliderCenterX + halfWidth, bottomLeftY, colliderCenterX + halfWidth, colliderCenterY + halfHeight, 1.0f, 0.0f, 0.0f);
			App::DrawLine(colliderCenterX + halfWidth, colliderCenterY + halfHeight, bottomLeftX, colliderCenterY + halfHeight, 1.0f, 0.0f, 0.0f);
			App::DrawLine(bottomLeftX, colliderCenterY + halfHeight, bottomLeftX, bottomLeftY, 1.0f, 0.0f, 0.0f);

			// Debug
			static std::string text;
			// Draw line around player2 and Bigball for spring
			static Vector2 player1Position;
			static Vector2 bigBallPosition;
			if (entity.HasTag("Player2"))
			{
				text = "Player2 velocity: " + entity.GetComponent<RigidBodyComponent>().velocity.ToString();
				player1Position = transform.position;
			}
			else if (entity.HasTag("BigBall"))
			{
				bigBallPosition = transform.position;
			}

			App::Print(50.f, 80.f, text.c_str(), 1, 0, 0, GLUT_BITMAP_HELVETICA_12);

			if (player1Position != Vector2() && bigBallPosition != Vector2())
			{
				App::DrawLine(bigBallPosition.x, bigBallPosition.y, player1Position.x, player1Position.y, 1.0f, 1.0f, 0.0f);
			}
		}
	}
};