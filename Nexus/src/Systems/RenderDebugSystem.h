#pragma once

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/ColliderTypeComponent.h"
#include "src/Components/PolygonColliderComponent.h"
#include "src/Components/CircleColliderComponent.h"
#include "src/Components/JointConstraintComponent.h"

#include "src/Physics/Contact.h"

#include "src/Utils/GraphicsUtils.h"

class RenderDebugSystem : public System
{
public:
	RenderDebugSystem()
	{
		RequireComponent<TransformComponent>();
		// RequireComponent<ColliderTypeComponent>();
	}

	void Update(const Camera& camera) const
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& transform = entity.GetComponent<TransformComponent>();
			if (entity.HasComponent<ColliderTypeComponent>())
			{
				auto& colliderType = entity.GetComponent<ColliderTypeComponent>();

				// Draw colliders
				switch (colliderType.type)
				{
					case ColliderType::Box:
						DrawBoxCollider(entity, camera);
						break;
					case ColliderType::Circle:
						DrawCircleCollider(entity, camera);
						break;
					case ColliderType::Polygon:
						DrawPolygonCollider(entity, camera);
						break;
				}

				// Draw contact info
				for (const auto& contact : colliderType.contacts)
				{
					const Vector2 startScreen = Camera::WorldToScreen(contact.startContactPoint, camera);
					const Vector2 endScreen = Camera::WorldToScreen(contact.endContactPoint, camera);
					const Vector2 normalEndScreen = Camera::WorldToScreen(
						contact.startContactPoint + contact.collisionNormal * 15,
						camera
					);

					Graphics::DrawCircle(Vector2(startScreen.x, startScreen.y), 3, 36, Color(Colors::RED));
					Graphics::DrawCircle(Vector2(endScreen.x, endScreen.y), 3, 36, Color(Colors::RED));
					Graphics::DrawLine(
						Vector2(startScreen.x, startScreen.y),
						Vector2(normalEndScreen.x, normalEndScreen.y),
						Color(Colors::RED)
					);
				}
				colliderType.contacts.clear();
			}

			// Debug lines for Spring relationship
			for (auto connectedEntity : entity.GetEntitiesByRelationshipTag("Spring"))
			{
				const auto& connectedEntityTransform = connectedEntity.GetComponent<TransformComponent>();
				const Vector2 start = Camera::WorldToScreen(transform.position, camera);
				const Vector2 end = Camera::WorldToScreen(connectedEntityTransform.position, camera);
				Graphics::DrawLine(
					Vector2(start.x, start.y),
					Vector2(end.x, end.y)
				);
			}

			// Debug lines for Joint Constraint relationship
			if (entity.HasComponent<JointConstraintComponent>())
			{
				auto& jointComponent = entity.GetComponent<JointConstraintComponent>();
				const Vector2 start = Camera::WorldToScreen(
					jointComponent.a.GetComponent<TransformComponent>().position,
					camera
				);
				const Vector2 end = Camera::WorldToScreen(
					jointComponent.b.GetComponent<TransformComponent>().position,
					camera
				);
				Graphics::DrawLine(
					Vector2(start.x, start.y),
					Vector2(end.x, end.y),
					Color(Colors::GRAY)
				);
			}
		}
	}

	static void DrawBoxCollider(const Entity& entity, const Camera& camera)
	{
		const auto& collider = entity.GetComponent<BoxColliderComponent>();
		const auto& vertices = Camera::TransformVertices(collider.globalVertices, camera);

		Graphics::DrawPolygon(vertices);
	}

	static void DrawCircleCollider(const Entity& entity, const Camera& camera)
	{
		const auto& transform = entity.GetComponent<TransformComponent>();
		const auto& collider = entity.GetComponent<CircleColliderComponent>();

		Vector2 centerScreen = Camera::WorldToScreen(collider.globalCenter, camera);
		Graphics::DrawCircle(
			Vector2(centerScreen.x, centerScreen.y),
			collider.radius, // Note: might need to scale radius based on camera zoom if implemented
			36
		);

		Graphics::DrawCircle(collider.globalCenter, collider.radius, 36);

		// For rotation line, using rotation angle to find the circle's edge
		const float outlineX = collider.radius * cos(transform.rotation) + collider.globalCenter.x;
		const float outlineY = collider.radius * sin(transform.rotation) + collider.globalCenter.y;
		const Vector2 outlineScreen = Camera::WorldToScreen(Vector2(outlineX, outlineY), camera);
		Graphics::DrawLine(
			Vector2(centerScreen.x, centerScreen.y),
			Vector2(outlineScreen.x, outlineScreen.y)
		);

	}

	static void DrawPolygonCollider(const Entity& entity, const Camera& camera)
	{
		const auto& collider = entity.GetComponent<PolygonColliderComponent>();
		const auto transformedVertices = Camera::TransformVertices(collider.globalVertices, camera);
		Graphics::DrawPolygon(transformedVertices);
	}
};