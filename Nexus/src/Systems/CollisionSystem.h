#pragma once

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
			if (IsCircleCircleCollision(a, b))
			{
				Logger::Log("CircleCircleCollision() Entity " + std::to_string(a.GetId()) + " is colliding with entity " + std::to_string(b.GetId()));
				eventManager->EmitEvent<CollisionEvent>(a, b);
			}
		}
		// Box-Box collision
		else if (aType == ColliderType::Box && bType == ColliderType::Box)
		{
			if (IsBoxBoxCollision(a, b))
			{
				Logger::Log("BoxBoxCollision(): Entity " + std::to_string(a.GetId()) + " is colliding with entity " + std::to_string(b.GetId()));
				eventManager->EmitEvent<CollisionEvent>(a, b);
			}
		}
		// Polygon-Polygon collision
		else if (aType == ColliderType::Polygon && bType == ColliderType::Polygon)
		{
			if (IsPolygonPolygonCollision(a, b))
			{
				Logger::Log("PolygonPolygonCollision(): Entity " + std::to_string(a.GetId()) + " is colliding with entity " + std::to_string(b.GetId()));
				eventManager->EmitEvent<CollisionEvent>(a, b);
			}
		}
		// TODO: collision between entities with different collider types
	}

	static bool IsCircleCircleCollision(const Entity a, const Entity b)
	{
		const auto& aCircleCollider = a.GetComponent<CircleColliderComponent>();
		const auto& bCircleCollider = b.GetComponent<CircleColliderComponent>();

		const Vector2 ab = bCircleCollider.globalCenter - aCircleCollider.globalCenter;
		const float radiusSum = aCircleCollider.radius + bCircleCollider.radius;

		return ab.MagnitudeSquared() <= (radiusSum * radiusSum);
	}

	static bool IsBoxBoxCollision(const Entity a, const Entity b)
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
		return PhysicsEngine::IsAABBCollision(
			aBottomLeftX, aBottomLeftY, aBoxCollider.width, aBoxCollider.height,
			bBottomLeftX, bBottomLeftY, bBoxCollider.width, bBoxCollider.height
		);
	}

	static bool IsPolygonPolygonCollision(const Entity a, const Entity b)
	{
		const auto& aPolygon = a.GetComponent<PolygonColliderComponent>();
		const auto& bPolygon = b.GetComponent<PolygonColliderComponent>();

		return PhysicsEngine::IsSATCollision(aPolygon.globalVertices, bPolygon.globalVertices);
	}
};