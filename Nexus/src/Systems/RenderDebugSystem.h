#pragma once

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/ColliderTypeComponent.h"
#include "src/Components/PolygonColliderComponent.h"
#include "src/Components/CircleColliderComponent.h"

#include "src/Utils/GraphicsUtils.h"

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
					DrawBoxCollider(entity);
					break;
				case ColliderType::Circle:
					DrawCircleCollider(entity);
					break;
				case ColliderType::Polygon:
					DrawPolygonCollider(entity);
					break;
			}

			// Debug rotation
			static std::string text;
			if (entity.HasTag("Debug"))
			{
				text = "Debug rotation: " + std::to_string(entity.GetComponent<TransformComponent>().rotation);
			}
			Graphics::PrintText(text, Vector2(50.0f, 80.0f));

			// Debug lines for Spring relationship
			for (auto connectedEntity : entity.GetEntitiesByRelationshipTag("Spring"))
			{
				const auto& connectedEntityTransform = connectedEntity.GetComponent<TransformComponent>();
				Graphics::DrawLine(connectedEntityTransform.position, transform.position, 1.0f, 1.0f, 0.0f);
			}
		}
	}

	static void DrawBoxCollider(const Entity& entity)
	{
		const auto& collider = entity.GetComponent<BoxColliderComponent>();
		const auto& vertices = collider.globalVertices;

		Graphics::DrawPolygon(vertices);
	}

	static void DrawCircleCollider(const Entity& entity)
	{
		const auto& transform = entity.GetComponent<TransformComponent>();
		const auto& collider = entity.GetComponent<CircleColliderComponent>();

		const Vector2 center = transform.position + collider.offset;

		Graphics::DrawCircle(center, collider.radius, 36, 1.0f, 1.0f, 1.0f);

		// For rotation line, using rotation angle to find the circle's edge
		const float outlineX = collider.radius * cos(transform.rotation) + center.x;
		const float outlineY = collider.radius * sin(transform.rotation) + center.y;
		Graphics::DrawLine(center, Vector2(outlineX, outlineY), 1.0f, 1.0f, 1.0f);

	}

	static void DrawPolygonCollider(const Entity& entity)
	{
		const auto& collider = entity.GetComponent<PolygonColliderComponent>();
		Graphics::DrawPolygon(collider.globalVertices, 1.0f, 1.0f, 1.0f);
	}
};