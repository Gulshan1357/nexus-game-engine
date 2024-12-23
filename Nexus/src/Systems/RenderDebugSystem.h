#pragma once

#include "../App/app.h"

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/ColliderTypeComponent.h"
#include "src/Components/PolygonColliderComponent.h"
#include "src/Components/CircleColliderComponent.h"

class RenderDebugSystem : public System
{
public:
	RenderDebugSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<ColliderTypeComponent>();
	}

	void Update() const
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& transform = entity.GetComponent<TransformComponent>();

			const auto& colliderType = entity.GetComponent<ColliderTypeComponent>();

			switch (colliderType.type)
			{
				case ColliderType::Box:
					DrawBox(entity);
					break;
				case ColliderType::Circle:
					DrawCircle(entity);
					break;
				case ColliderType::Polygon:
					DrawPolygon(entity);
					break;
			}

			// Debug rotation
			static std::string text;
			if (entity.HasTag("Debug"))
			{
				text = "Player2 rotation: " + std::to_string(entity.GetComponent<TransformComponent>().rotation);
			}
			App::Print(50.f, 80.f, text.c_str(), 1, 0, 0, GLUT_BITMAP_HELVETICA_12);

			// Debug lines for Spring relationship
			for (auto connectedEntity : entity.GetEntitiesByRelationshipTag("Spring"))
			{
				const auto& connectedEntityTransform = connectedEntity.GetComponent<TransformComponent>();
				App::DrawLine(connectedEntityTransform.position.x, connectedEntityTransform.position.y, transform.position.x, transform.position.y, 1.0f, 1.0f, 0.0f);
			}
		}
	}

	static void DrawBox(const Entity& entity)
	{
		const auto& transform = entity.GetComponent<TransformComponent>();
		const auto& collider = entity.GetComponent<BoxColliderComponent>();

		const float colliderCenterX = transform.position.x + collider.offset.x;
		const float colliderCenterY = transform.position.y + collider.offset.y;

		const float halfWidth = static_cast<float>(collider.width) / 2.0f;
		const float halfHeight = static_cast<float>(collider.height) / 2.0f;

		const float bottomLeftX = colliderCenterX - halfWidth;
		const float bottomLeftY = colliderCenterY - halfHeight;

		// Draw a rectangle around the collider
		App::DrawLine(bottomLeftX, bottomLeftY, colliderCenterX + halfWidth, bottomLeftY, 1.0f, 1.0f, 1.0f);
		App::DrawLine(colliderCenterX + halfWidth, bottomLeftY, colliderCenterX + halfWidth, colliderCenterY + halfHeight, 1.0f, 1.0f, 1.0f);
		App::DrawLine(colliderCenterX + halfWidth, colliderCenterY + halfHeight, bottomLeftX, colliderCenterY + halfHeight, 1.0f, 1.0f, 1.0f);
		App::DrawLine(bottomLeftX, colliderCenterY + halfHeight, bottomLeftX, bottomLeftY, 1.0f, 1.0f, 1.0f);
	}

	static void DrawCircle(const Entity& entity)
	{
		const auto& transform = entity.GetComponent<TransformComponent>();
		const auto& collider = entity.GetComponent<CircleColliderComponent>();

		const float centerX = transform.position.x + collider.offset.x;
		const float centerY = transform.position.y + collider.offset.y;

		constexpr int segments = 36;
		const float radius = collider.radius;

		for (int i = 0; i < segments; ++i)
		{
			// A full circle in radians = 2 * π * (360 degrees in radians)
			// => Angle step (in radians) = 2 * π / segments
			// => Angle (w.r.t to a baseline for each segment i) = 2 * π * (i / segments)
			const float theta1 = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
			const float theta2 = 2.0f * PI * static_cast<float>(i + 1) / static_cast<float>(segments);

			// Coordinates of segment's first point
			const float x1 = centerX + radius * cosf(theta1);
			const float y1 = centerY + radius * sinf(theta1);

			// Coordinates of segment's second point
			const float x2 = centerX + radius * cosf(theta2);
			const float y2 = centerY + radius * sinf(theta2);

			App::DrawLine(x1, y1, x2, y2, 1.0f, 1.0f, 1.0f);
		}

		// For rotation line, using rotation angle to find the circle's edge
		const float outlineX = radius * cos(transform.rotation) + centerX;
		const float outlineY = radius * sin(transform.rotation) + centerY;
		App::DrawLine(centerX, centerY, outlineX, outlineY, 1.0f, 1.0f, 1.0f);
	}

	static void DrawPolygon(const Entity& entity)
	{
		const auto& transform = entity.GetComponent<TransformComponent>();
		const auto& collider = entity.GetComponent<PolygonColliderComponent>();
		const auto& vertices = collider.vertices;

		const float offsetX = transform.position.x + collider.offset.x;
		const float offsetY = transform.position.y + collider.offset.y;

		for (size_t i = 0; i < vertices.size(); ++i)
		{
			const auto& v1 = vertices[i];
			const auto& v2 = vertices[(i + 1) % vertices.size()];

			App::DrawLine(v1.x + offsetX, v1.y + offsetY, v2.x + offsetX, v2.y + offsetY, 1.0f, 1.0f, 1.0f);
		}
	}
};