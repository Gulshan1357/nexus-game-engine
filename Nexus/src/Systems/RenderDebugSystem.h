#pragma once

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/ColliderTypeComponent.h"
#include "src/Components/PolygonColliderComponent.h"
#include "src/Components/CircleColliderComponent.h"

#include "src/Physics/Contact.h"

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
			auto& colliderType = entity.GetComponent<ColliderTypeComponent>();

			// Draw colliders
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

			// Draw contact info
			Graphics::DrawCircle(colliderType.contactInfo.startContactPoint, 3, 36, Color(Colors::RED));
			Graphics::DrawCircle(colliderType.contactInfo.endContactPoint, 3, 36, Color(Colors::RED));
			Graphics::DrawLine(colliderType.contactInfo.startContactPoint, colliderType.contactInfo.startContactPoint + colliderType.contactInfo.collisionNormal * 15, Color(Colors::RED));
			colliderType.contactInfo = Contact();

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
				Graphics::DrawLine(connectedEntityTransform.position, transform.position);
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

		Graphics::DrawCircle(collider.globalCenter, collider.radius, 36);

		// For rotation line, using rotation angle to find the circle's edge
		const float outlineX = collider.radius * cos(transform.rotation) + collider.globalCenter.x;
		const float outlineY = collider.radius * sin(transform.rotation) + collider.globalCenter.y;
		Graphics::DrawLine(collider.globalCenter, Vector2(outlineX, outlineY));

	}

	static void DrawPolygonCollider(const Entity& entity)
	{
		const auto& collider = entity.GetComponent<PolygonColliderComponent>();
		Graphics::DrawPolygon(collider.globalVertices);
	}
};