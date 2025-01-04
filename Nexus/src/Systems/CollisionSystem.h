#pragma once

#include <optional>
#include <limits>

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/ColliderTypeComponent.h"

#include "src/EventManagement/EventManager.h"
#include "src/Events/CollisionEvent.h"
#include "src/Physics/Contact.h"

#include "src/Physics/PhysicsEngine.h"
#include "src/Utils/GraphicsUtils.h"

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
			auto& aColliderType = a.GetComponent<ColliderTypeComponent>();

			for (auto j = i + 1; j != entities.end(); ++j)
			{
				Entity b = *j;
				auto& bColliderType = b.GetComponent<ColliderTypeComponent>();
				CheckAndHandleCollision(eventManager, a, b, aColliderType, bColliderType);
			}
		}
	}

	static void CheckAndHandleCollision(const std::unique_ptr<EventManager>& eventManager, Entity a, Entity b, ColliderTypeComponent& aType, ColliderTypeComponent& bType)
	{
		// Circle-Circle collision
		if (aType.type == ColliderType::Circle && bType.type == ColliderType::Circle)
		{
			auto collisionInfo = GetCircleCircleCollisionInfo(a, b);

			if (collisionInfo.has_value())
			{
				Logger::Log("Circle-Circle collision between Entity " + std::to_string(a.GetId()) + " and Entity " + std::to_string(b.GetId()));
				Logger::Log("Collision Depth: " + std::to_string(collisionInfo.value().penetrationDepth));
				aType.contactInfo = collisionInfo.value();
				bType.contactInfo = collisionInfo.value();
				eventManager->EmitEvent<CollisionEvent>(a, b, collisionInfo.value());
			}
		}
		// Box-Box or Polygon-Polygon collision
		else if ((aType.type == ColliderType::Box || aType.type == ColliderType::Polygon) && (bType.type == ColliderType::Box || bType.type == ColliderType::Polygon))
		{
			auto collisionInfo = GetPolygonPolygonCollisionInfo(a, b);
			if (collisionInfo.has_value())
			{
				Logger::Log("Polygon-Polygon collision between Entity " + std::to_string(a.GetId()) + " and Entity " + std::to_string(b.GetId()));
				Logger::Log("Collision Depth: " + std::to_string(collisionInfo.value().penetrationDepth));
				aType.contactInfo = collisionInfo.value();
				bType.contactInfo = collisionInfo.value();
				eventManager->EmitEvent<CollisionEvent>(a, b, collisionInfo.value());
			}
		}

		// Circle-Polygon collision
		else if (aType.type == ColliderType::Circle && (bType.type == ColliderType::Polygon || bType.type == ColliderType::Box))
		{
			auto collisionInfo = GetCirclePolygonCollisionInfo(a, b);
			if (collisionInfo.has_value())
			{
				Logger::Log("Circle-Polygon collision between Entity " + std::to_string(a.GetId()) + " and Entity " + std::to_string(b.GetId()));
				Logger::Log("Collision Depth: " + std::to_string(collisionInfo.value().penetrationDepth));
				aType.contactInfo = collisionInfo.value();
				bType.contactInfo = collisionInfo.value();
				eventManager->EmitEvent<CollisionEvent>(a, b, collisionInfo.value());
			}
		}

		// Polygon-Circle collision
		else if ((aType.type == ColliderType::Polygon || aType.type == ColliderType::Box) && bType.type == ColliderType::Circle)
		{
			auto collisionInfo = GetCirclePolygonCollisionInfo(b, a);
			if (collisionInfo.has_value())
			{
				Logger::Log("Polygon-Circle collision between Entity " + std::to_string(a.GetId()) + " and Entity " + std::to_string(b.GetId()));
				Logger::Log("Collision Depth: " + std::to_string(collisionInfo.value().penetrationDepth));
				aType.contactInfo = collisionInfo.value();
				bType.contactInfo = collisionInfo.value();
				eventManager->EmitEvent<CollisionEvent>(a, b, collisionInfo.value());
			}
		}
	}

	// Collision detection between circle-circle
	static std::optional<Contact> GetCircleCircleCollisionInfo(const Entity a, const Entity b)
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

		const Vector2 collisionNormal = ab.Normalize();

		// Start contact point is the point of circle 'b' inside 'a'. So that is the position of b minus the scaled normal. Subtracting because normal ab is from a to b
		const Vector2 startContactPoint = bCircleCollider.globalCenter - (collisionNormal * bCircleCollider.radius);

		// End Contact point is the point of circle 'a' inside 'b'. So that is the position of a plus the scaled normal.
		const Vector2 endContactPoint = aCircleCollider.globalCenter + (collisionNormal * aCircleCollider.radius);

		const float penetrationDepth = (endContactPoint - startContactPoint).Magnitude();

		return Contact(startContactPoint, endContactPoint, collisionNormal, penetrationDepth);
	}

	// Collision detection between box-box, polygon-polygon and box-polygon
	static std::optional<Contact> GetPolygonPolygonCollisionInfo(const Entity a, const Entity b)
	{
		const auto& aCollider = a.GetComponent<ColliderTypeComponent>();
		const auto& bCollider = b.GetComponent<ColliderTypeComponent>();

		std::vector<Vector2> aGlobalVertices;
		std::vector<Vector2> bGlobalVertices;

		// Retrieve polygon (or box) properties of entity 'a'
		if (aCollider.type == ColliderType::Box)
		{
			aGlobalVertices = a.GetComponent<BoxColliderComponent>().globalVertices;
		}
		else if (aCollider.type == ColliderType::Polygon)
		{
			aGlobalVertices = a.GetComponent<PolygonColliderComponent>().globalVertices;
		}

		// Retrieve polygon (or box) properties of entity 'b'
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

		return Contact(startContactPoint, endContactPoint, collisionNormal, penetration);
	}

	// Collision detection between circle-polygon(or box)
	static std::optional<Contact> GetCirclePolygonCollisionInfo(const Entity circleEntity, const Entity polygonEntity)
	{
		// Retrieve circle properties
		const auto& circleCollider = circleEntity.GetComponent<ColliderTypeComponent>();
		Vector2 circleCenter = circleEntity.GetComponent<CircleColliderComponent>().globalCenter;
		float circleRadius = circleEntity.GetComponent<CircleColliderComponent>().radius;

		// Retrieve polygon (or box) properties
		const auto& polygonCollider = polygonEntity.GetComponent<ColliderTypeComponent>();
		std::vector<Vector2> polygonVertices; // Global vertices
		if (polygonCollider.type == ColliderType::Box)
		{
			polygonVertices = polygonEntity.GetComponent<BoxColliderComponent>().globalVertices;
		}
		else if (polygonCollider.type == ColliderType::Polygon)
		{
			polygonVertices = polygonEntity.GetComponent<PolygonColliderComponent>().globalVertices;
		}

		//------------------------------------------------------------------------
		// Find the closed edge of polygon to the circle's center
		//------------------------------------------------------------------------
		Vector2 closestEdgeStart;
		Vector2 closestEdgeEnd;

		bool isCircleCenterOutsidePolygon = false;
		float maxProjection = std::numeric_limits<float>::lowest(); // Maximum Projection between an edge to the circle's center

		// Loop all the edges of the polygon to find the nearest edge to the circle center
		for (size_t i = 0; i < polygonVertices.size(); i++)
		{
			Vector2 edgeStart = polygonVertices[i];
			Vector2 edgeEnd = polygonVertices[(i + 1) % polygonVertices.size()];
			Vector2 edge = edgeEnd - edgeStart;
			Vector2 edgeNormal = edge.Normal();

			// Project the circle center onto the edge normal
			Vector2 vertexToCircleCenter = circleCenter - edgeStart;
			float projection = vertexToCircleCenter.Dot(edgeNormal);

			if (projection > 0.f)
			{
				// Circle center is outside the polygon
				maxProjection = projection;
				closestEdgeStart = edgeStart;
				closestEdgeEnd = edgeEnd;
				isCircleCenterOutsidePolygon = true;
			}
			else
			{
				// Circle center is inside the polygon, find the min edge that has the least negative projection
				if (projection > maxProjection)
				{
					maxProjection = projection;
					closestEdgeStart = edgeStart;
					closestEdgeEnd = edgeEnd;
				}
			}
		}

		if (isCircleCenterOutsidePolygon)
		{
			// The circle center could be in one of three regions. The region where:
			// 1. the top edge vertex of the closest edge is the closest point to the circle center.
			// 2. the bottom edge vertex of the closest edge is the closet point to the circle center.
			// 3. A vertex along the closest edge is the closest point to the circle center.

			//------------------------------------------------------------------------
			// Check if we are inside region 1 (Closest to the start of the closest edge)
			//------------------------------------------------------------------------

			Vector2 toCircleFromStart = circleCenter - closestEdgeStart; // Vector from the nearest vertex to the circle center
			Vector2 edgeStartToEnd = closestEdgeEnd - closestEdgeStart; // the nearest edge
			if (toCircleFromStart.Dot(edgeStartToEnd) < 0)
			{
				if (toCircleFromStart.Magnitude() > circleRadius)
				{
					return std::nullopt; // No collision
				}
				else
				{
					// Collision detected in region 1
					Vector2 collisionNormal = toCircleFromStart.Normalize();
					float penetrationDepth = circleRadius - toCircleFromStart.Magnitude();

					// // Blending the penetration depth between vertex and edge regions to avoid sudden jumps (find a better way)
					// float vertexDepth = circleRadius - toCircleFromStart.Magnitude();
					// float edgeDepth = circleRadius - maxProjection;
					// float penetrationDepth = std::min<float>(vertexDepth, edgeDepth);

					Vector2 startContactPoint = circleCenter - (collisionNormal * circleRadius);
					Vector2 endContactPoint = startContactPoint + (collisionNormal * penetrationDepth);
					return Contact(startContactPoint, endContactPoint, collisionNormal, penetrationDepth);
				}
			}
			else
			{
				//------------------------------------------------------------------------
				// Check if we are inside region 2 (Closest to the end of the closest edge)
				//------------------------------------------------------------------------
				Vector2 toCircleFromEnd = circleCenter - closestEdgeEnd; // Vector from the next nearest vertex (in opposite direction) to the circle center
				Vector2 edgeEndToStart = closestEdgeStart - closestEdgeEnd; // the nearest edge
				if (toCircleFromEnd.Dot(edgeEndToStart) < 0)
				{
					if (toCircleFromEnd.Magnitude() > circleRadius)
					{
						return std::nullopt; // No collision
					}
					else
					{
						// Collision detected in region 2
						Vector2 collisionNormal = toCircleFromEnd.Normalize();
						float penetrationDepth = circleRadius - toCircleFromEnd.Magnitude();
						Vector2 startContactPoint = circleCenter - (collisionNormal * circleRadius);
						Vector2 endContactPoint = startContactPoint + (collisionNormal * penetrationDepth);
						return Contact(startContactPoint, endContactPoint, collisionNormal, penetrationDepth);
					}
				}
				else
				{
					//------------------------------------------------------------------------
					// Inside region 3
					//------------------------------------------------------------------------
					if (maxProjection > circleRadius)
					{
						// Distance between the closest distance and the circle center is greater than the radius
						return std::nullopt; // No collision
					}
					else
					{
						// Collision detected in region 3
						Vector2 collisionNormal = (closestEdgeEnd - closestEdgeStart).Normal();
						float penetrationDepth = circleRadius - maxProjection;
						Vector2 startContactPoint = circleCenter - (collisionNormal * circleRadius);
						Vector2 endContactPoint = startContactPoint + (collisionNormal * penetrationDepth);
						return Contact(startContactPoint, endContactPoint, collisionNormal, penetrationDepth);
					}
				}
			}
		}
		else
		{
			// Circle center is inside the polygon
			Vector2 collisionNormal = (closestEdgeEnd - closestEdgeStart).Normal();
			float penetrationDepth = circleRadius - maxProjection;
			Vector2 startContactPoint = circleCenter - (collisionNormal * circleRadius);
			Vector2 endContactPoint = startContactPoint + (collisionNormal * penetrationDepth);
			return Contact(startContactPoint, endContactPoint, collisionNormal, penetrationDepth);
		}
	}
};