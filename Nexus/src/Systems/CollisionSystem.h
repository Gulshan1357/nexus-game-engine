#pragma once

#include <optional>
#include <limits>

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
				CheckAndHandleCollision(eventManager, a, b, aColliderType.type, bColliderType.type);
			}
		}
	}

	static void CheckAndHandleCollision(const std::unique_ptr<EventManager>& eventManager, Entity a, Entity b, const ColliderType aType, const ColliderType bType)
	{
		// Circle-Circle collision
		if (aType == ColliderType::Circle && bType == ColliderType::Circle)
		{
			auto collisionInfo = GetCircleCircleCollisionInfo(a, b);
			if (collisionInfo.has_value())
			{
				Logger::Log("Circle-Circle collision between Entity " + std::to_string(a.GetId()) + " and Entity " + std::to_string(b.GetId()));
				eventManager->EmitEvent<CollisionEvent>(a, b, collisionInfo.value());
			}
		}
		// Box-Box or Polygon-Polygon collision
		else if ((aType == ColliderType::Box || aType == ColliderType::Polygon) && (bType == ColliderType::Box || bType == ColliderType::Polygon))
		{
			auto collisionInfo = GetPolygonPolygonCollisionInfo(a, b);
			if (collisionInfo.has_value())
			{
				Logger::Log("Polygon-Polygon collision between Entity " + std::to_string(a.GetId()) + " and Entity " + std::to_string(b.GetId()));
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

		Vector2 ab = bCircleCollider.globalCenter - aCircleCollider.globalCenter;
		const float radiusSum = aCircleCollider.radius + bCircleCollider.radius;

		if (ab.MagnitudeSquared() > (radiusSum * radiusSum))
		{
			return std::nullopt; // No collision
		}

		// If there is a collision then calculate and return contact info

		Vector2 collisionNormal = ab.Normalize();

		// Start contact point is the point of circle 'b' inside 'a'. So that is the position of b minus the scaled normal. Subtracting because normal ab is from a to b
		Vector2 startContactPoint = bCircleCollider.globalCenter - (collisionNormal * bCircleCollider.radius);

		// End Contact point is the point of circle 'a' inside 'b'. So that is the position of a plus the scaled normal.
		Vector2 endContactPoint = aCircleCollider.globalCenter + (collisionNormal * aCircleCollider.radius);

		float penetrationDepth = (endContactPoint - startContactPoint).Magnitude();

		return ContactInfo(startContactPoint, endContactPoint, collisionNormal, penetrationDepth);
	}

	static std::optional<ContactInfo> GetPolygonPolygonCollisionInfo(const Entity a, const Entity b)
	{
		std::vector<Vector2> aGlobalVertices;
		std::vector<Vector2> bGlobalVertices;

		// Logger::Log("inside polygon-polygon info");

		const auto aCollider = a.GetComponent<ColliderTypeComponent>();
		if (aCollider.type == ColliderType::Box)
		{
			aGlobalVertices = a.GetComponent<BoxColliderComponent>().globalVertices;
		}
		else if (aCollider.type == ColliderType::Polygon)
		{
			aGlobalVertices = a.GetComponent<PolygonColliderComponent>().globalVertices;
		}

		const auto bCollider = b.GetComponent<ColliderTypeComponent>();
		if (bCollider.type == ColliderType::Box)
		{
			bGlobalVertices = b.GetComponent<BoxColliderComponent>().globalVertices;
		}
		else if (bCollider.type == ColliderType::Polygon)
		{
			bGlobalVertices = b.GetComponent<PolygonColliderComponent>().globalVertices;
		}

		// Values will be calculated by IsSATCollision()
		Vector2 startContactPoint = {};
		Vector2 endContactPoint = {};
		float penetration = std::numeric_limits<float>::infinity();
		Vector2 collisionNormal;

		if (!PhysicsEngine::IsSATCollision(aGlobalVertices, bGlobalVertices, startContactPoint, endContactPoint, collisionNormal, penetration))
		{
			return std::nullopt;
		}

		return ContactInfo(startContactPoint, endContactPoint, collisionNormal, penetration);
	}
};