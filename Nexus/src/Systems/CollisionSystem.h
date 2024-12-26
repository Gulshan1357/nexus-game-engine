#pragma once

#include <optional>

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/ColliderTypeComponent.h"

#include "src/EventManagement/EventManager.h"
#include "src/Events/CollisionEvent.h"

#include "src/Physics/PhysicsEngine.h"

#include "src/Utils/Logger.h"

class CollisionSystem : public System
{
public:
	CollisionSystem()
	{
		RequireComponent<ColliderTypeComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(const std::unique_ptr<EventManager>& eventManager) const
	{
		auto entities = GetSystemEntities();

		// Loop all entities that the system is interested in
		for (auto i = entities.begin(); i != entities.end(); ++i)
		{
			Entity a = *i;
			const auto& aColliderType = a.GetComponent<ColliderTypeComponent>();

			for (auto j = i + 1; j != entities.end(); ++j)
			{
				Entity b = *j;
				const auto& bColliderType = b.GetComponent<ColliderTypeComponent>();
				HandleCollision(eventManager, a, b, aColliderType.type, bColliderType.type);
			}
		}
	}

	static void HandleCollision(const std::unique_ptr<EventManager>& eventManager, Entity a, Entity b, const ColliderType aType, const ColliderType bType)
	{
		// Circle-Circle collision
		if (aType == ColliderType::Circle && bType == ColliderType::Circle)
		{
			auto collisionInfo = GetCircleCircleCollisionInfo(a, b);
			if (collisionInfo.has_value())
			{
				Logger::Log("CircleCircleCollision() Entity " + std::to_string(a.GetId()) + " is colliding with entity " + std::to_string(b.GetId()));
				eventManager->EmitEvent<CollisionEvent>(a, b, collisionInfo.value());
			}
		}
		// Box-Box collision
		else if (aType == ColliderType::Box && bType == ColliderType::Box)
		{
			auto collisionInfo = GetBoxBoxCollisionInfo(a, b);
			if (collisionInfo.has_value())
			{
				Logger::Log("BoxBoxCollision(): Entity " + std::to_string(a.GetId()) + " is colliding with entity " + std::to_string(b.GetId()));
				eventManager->EmitEvent<CollisionEvent>(a, b, collisionInfo.value());
			}
		}
		// Polygon-Polygon collision
		else if (aType == ColliderType::Polygon && bType == ColliderType::Polygon)
		{
			auto collisionInfo = GetPolygonPolygonCollisionInfo(a, b);
			if (collisionInfo.has_value())
			{
				Logger::Log("PolygonPolygonCollision(): Entity " + std::to_string(a.GetId()) + " is colliding with entity " + std::to_string(b.GetId()));
				eventManager->EmitEvent<CollisionEvent>(a, b, collisionInfo.value());
			}
		}
		// TODO: collision between entities with different collider types
	}

	// If a collision is detected, return the contact information; otherwise, return std::nullopt
	static std::optional<ContactInfo> GetCircleCircleCollisionInfo(const Entity a, const Entity b)
	{
		const auto& aCircleCollider = a.GetComponent<CircleColliderComponent>();
		const auto& bCircleCollider = b.GetComponent<CircleColliderComponent>();

		const Vector2 ab = bCircleCollider.globalCenter - aCircleCollider.globalCenter;
		const float radiusSum = aCircleCollider.radius + bCircleCollider.radius;

		if (ab.MagnitudeSquared() > (radiusSum * radiusSum))
		{
			return std::nullopt; // No collision
		}

		// If there is the collision then calculate and return contact info

		// Start contact point is the point of circle 'b' inside 'a'. So that is the position of b minus the scaled normal. Subtracting because normal ab is from a to b
		Vector2 startContactPoint = bCircleCollider.globalCenter - (ab * bCircleCollider.radius);

		// End Contact point is the point of circle 'a' inside 'b'. So that is the position of a plus the scaled normal.
		Vector2 endContactPoint = aCircleCollider.globalCenter + (ab * aCircleCollider.radius);

		float penetrationDepth = (endContactPoint - startContactPoint).Magnitude();

		return ContactInfo(startContactPoint, endContactPoint, ab, penetrationDepth);
	}

	static std::optional<ContactInfo> GetBoxBoxCollisionInfo(const Entity a, const Entity b)
	{
		const auto& aTransform = a.GetComponent<TransformComponent>();
		const auto& aBoxCollider = a.GetComponent<BoxColliderComponent>();

		const auto& bTransform = b.GetComponent<TransformComponent>();
		const auto& bBoxCollider = b.GetComponent<BoxColliderComponent>();

		// Calculate the bottom-left positions of entities 'a' and 'b'
		const float aBottomLeftX = aTransform.position.x + aBoxCollider.offset.x - static_cast<float>(aBoxCollider.width) / 2.0f;
		const float aBottomLeftY = aTransform.position.y + aBoxCollider.offset.y - static_cast<float>(aBoxCollider.height) / 2.0f;

		const float bBottomLeftX = bTransform.position.x + bBoxCollider.offset.x - static_cast<float>(bBoxCollider.width) / 2.0f;
		const float bBottomLeftY = bTransform.position.y + bBoxCollider.offset.y - static_cast<float>(bBoxCollider.height) / 2.0f;

		// Check for Axis-Aligned Bounding Box collision and return bool
		if (!PhysicsEngine::IsAABBCollision(
			aBottomLeftX, aBottomLeftY, aBoxCollider.width, aBoxCollider.height,
			bBottomLeftX, bBottomLeftY, bBoxCollider.width, bBoxCollider.height
		))
		{
			return std::nullopt;
		}

		// TODO: Calculate the Contact info and return that
		return ContactInfo();
	}

	static std::optional<ContactInfo> GetPolygonPolygonCollisionInfo(const Entity a, const Entity b)
	{
		const auto& aPolygon = a.GetComponent<PolygonColliderComponent>();
		const auto& bPolygon = b.GetComponent<PolygonColliderComponent>();

		float penetration;
		Vector2 collisionNormal;

		if (!PhysicsEngine::IsSATCollision(aPolygon.globalVertices, bPolygon.globalVertices, penetration, collisionNormal))
		{
			return std::nullopt;
		}

		// TODO: Very simplified contact point calculation (needs improvement)
		Vector2 startContactPoint = aPolygon.globalVertices[0];
		Vector2 endContactPoint = bPolygon.globalVertices[0];

		return ContactInfo(startContactPoint, endContactPoint, collisionNormal, penetration);
	}
};