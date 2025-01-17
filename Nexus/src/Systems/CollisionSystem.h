#pragma once

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

				std::vector<Contact> contacts;
				if (isColliding(a, b, aColliderType, bColliderType, contacts))
				{
					aColliderType.contacts = contacts; // For render debug
					bColliderType.contacts = contacts; // For render debug
					eventManager->EmitEvent<CollisionEvent>(a, b, contacts);
				}
			}
		}
	}

	static bool isColliding(const Entity a, const Entity b, const ColliderTypeComponent& aType, const ColliderTypeComponent& bType, std::vector<Contact>& contacts)
	{
		// Circle-Circle collision
		if (aType.type == ColliderType::Circle && bType.type == ColliderType::Circle)
		{
			if (IsCollidingCircleCircle(a, b, contacts))
			{
				// Logger::Log("Circle-Circle collision between Entity " + std::to_string(a.GetId()) + " and Entity " + std::to_string(b.GetId()));
				// Logger::Log("Collision Depth: ");
				// for (const auto& contact : contacts)
				// 	Logger::Log("Collision Depth: " + std::to_string(contact.penetrationDepth));
				return true;
			}
		}
		// Box-Box or Polygon-Polygon collision
		else if ((aType.type == ColliderType::Box || aType.type == ColliderType::Polygon) && (bType.type == ColliderType::Box || bType.type == ColliderType::Polygon))
		{
			if (IsCollidingPolygonPolygon(a, b, contacts))
			{
				// Logger::Log("Polygon-Polygon collision between Entity " + std::to_string(a.GetId()) + " and Entity " + std::to_string(b.GetId()));
				// Logger::Log("Collision Depth: ");
				// for (const auto& contact : contacts)
				// 	Logger::Log("Collision Depth: " + std::to_string(contact.penetrationDepth));
				return true;
			}
		}

		// Circle-Polygon collision
		else if (aType.type == ColliderType::Circle && (bType.type == ColliderType::Polygon || bType.type == ColliderType::Box))
		{
			if (IsCollidingCirclePolygon(a, b, contacts))
			{
				// Logger::Log("Circle-Polygon collision between Entity " + std::to_string(a.GetId()) + " and Entity " + std::to_string(b.GetId()));
				// Logger::Log("Collision Depth: ");
				// for (const auto& contact : contacts)
				// 	Logger::Log("Collision Depth: " + std::to_string(contact.penetrationDepth));
				return true;
			}
		}

		// Polygon-Circle collision
		else if ((aType.type == ColliderType::Polygon || aType.type == ColliderType::Box) && bType.type == ColliderType::Circle)
		{
			if (IsCollidingCirclePolygon(b, a, contacts))
			{
				// Logger::Log("Polygon-Circle collision between Entity " + std::to_string(a.GetId()) + " and Entity " + std::to_string(b.GetId()));
				// Logger::Log("Collision Depth: ");
				// for (const auto& contact : contacts)
				// 	Logger::Log("Collision Depth: " + std::to_string(contact.penetrationDepth));
				return true;
			}
		}
		return false;
	}

	// Collision detection between circle-circle
	static bool IsCollidingCircleCircle(const Entity a, const Entity b, std::vector<Contact>& outContacts)
	{
		const auto& aCircleCollider = a.GetComponent<CircleColliderComponent>();
		const auto& bCircleCollider = b.GetComponent<CircleColliderComponent>();

		Vector2 ab = bCircleCollider.globalCenter - aCircleCollider.globalCenter;
		const float radiusSum = aCircleCollider.radius + bCircleCollider.radius;

		if (ab.MagnitudeSquared() > (radiusSum * radiusSum))
		{
			return false; // No collision
		}

		// If there is a collision then calculate and add the contact info to the outContacts vector. There will only be one contact
		const Vector2 collisionNormal = ab.Normalize();

		// Start contact point is the point of circle 'b' inside 'a'. So that is the position of b minus the scaled normal. Subtracting because normal ab is from a to b
		const Vector2 startContactPoint = bCircleCollider.globalCenter - (collisionNormal * bCircleCollider.radius);

		// End Contact point is the point of circle 'a' inside 'b'. So that is the position of a plus the scaled normal.
		const Vector2 endContactPoint = aCircleCollider.globalCenter + (collisionNormal * aCircleCollider.radius);

		const float penetrationDepth = (endContactPoint - startContactPoint).Magnitude();

		outContacts.emplace_back(startContactPoint, endContactPoint, collisionNormal, penetrationDepth);
		return true;
	}

	// Collision detection between box-box, polygon-polygon and box-polygon
	static bool IsCollidingPolygonPolygon(const Entity a, const Entity b, std::vector<Contact>& outContacts)
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

		// // Values will be calculated by IsSATCollision()
		// Vector2 startContactPoint = {};
		// Vector2 endContactPoint = {};
		// float penetration = std::numeric_limits<float>::infinity();
		// Vector2 collisionNormal;
		//
		// Contact contact;
		if (!PhysicsEngine::IsSATCollision(aGlobalVertices, bGlobalVertices, outContacts))
		{
			return false;
		}

		// outContacts.emplace_back(contact);
		return true;
	}

	// Collision detection between circle-polygon(or box)
	static bool IsCollidingCirclePolygon(const Entity circleEntity, const Entity polygonEntity, std::vector<Contact>& outContacts)
	{
		// Retrieve circle properties
		auto& circleCollider = circleEntity.GetComponent<CircleColliderComponent>();
		Vector2 circleCenter = circleCollider.globalCenter;
		float circleRadius = circleCollider.radius;

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
					return false; // No collision
				}
				else
				{
					// Collision detected in region 1 (vertex)
					Vector2 vertexNormal = toCircleFromStart.Normalize();
					Vector2 edgeNormal = (closestEdgeEnd - closestEdgeStart).Normal();

					// Calculate blend factor based on how close we are to the edge
					float blendRegionSize = circleRadius * 0.5f;  // Match region 2
					float distanceAlongEdge = toCircleFromStart.Dot(edgeStartToEnd.Normalize());  // Use consistent direction
					float blendFactor = std::max<float>(0.0f, std::min<float>(1.0f, -distanceAlongEdge / blendRegionSize));

					// Simple blend between vertex and edge normal (like region 2)
					Vector2 collisionNormal = (vertexNormal * blendFactor + edgeNormal * (1.0f - blendFactor)).Normalize();

					// Blend penetration depths (match region 2's approach)
					float vertexDepth = circleRadius - toCircleFromStart.Magnitude();
					float edgeDepth = circleRadius - maxProjection;
					float penetrationDepth = std::min<float>(vertexDepth, edgeDepth);
					penetrationDepth *= (0.8f + 0.2f * (1.0f - blendFactor));  // Match region 2's scaling

					Vector2 startContactPoint = circleCenter - (collisionNormal * circleRadius);
					Vector2 endContactPoint = startContactPoint + (collisionNormal * penetrationDepth);
					outContacts.emplace_back(startContactPoint, endContactPoint, collisionNormal, penetrationDepth);
					return true;
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
						return false; // No collision
					}
					else
					{
						// Collision detected in region 2 (vertex)
						Vector2 vertexNormal = toCircleFromEnd.Normalize();
						Vector2 edgeNormal = (closestEdgeEnd - closestEdgeStart).Normal();

						// Calculate blend factor based on how close we are to the edge
						float blendRegionSize = circleRadius * 0.5f;
						float distanceAlongEdge = toCircleFromEnd.Dot(edgeEndToStart.Normalize());
						float blendFactor = std::max<float>(0.0f, std::min<float>(1.0f, -distanceAlongEdge / blendRegionSize));

						// Blend between vertex and edge normal
						Vector2 collisionNormal = (vertexNormal * blendFactor + edgeNormal * (1.0f - blendFactor)).Normalize();

						// Blend penetration depths
						float vertexDepth = circleRadius - toCircleFromEnd.Magnitude();
						float edgeDepth = circleRadius - maxProjection;
						float penetrationDepth = std::min<float>(vertexDepth, edgeDepth);
						penetrationDepth *= (0.8f + 0.2f * (1.0f - blendFactor));

						Vector2 startContactPoint = circleCenter - (collisionNormal * circleRadius);
						Vector2 endContactPoint = startContactPoint + (collisionNormal * penetrationDepth);
						outContacts.emplace_back(startContactPoint, endContactPoint, collisionNormal, penetrationDepth);
						return true;
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
						return false; // No collision
					}
					else
					{
						// Collision detected in region 3
						Vector2 collisionNormal = (closestEdgeEnd - closestEdgeStart).Normal();
						float penetrationDepth = circleRadius - maxProjection;
						Vector2 startContactPoint = circleCenter - (collisionNormal * circleRadius);
						Vector2 endContactPoint = startContactPoint + (collisionNormal * penetrationDepth);
						outContacts.emplace_back(startContactPoint, endContactPoint, collisionNormal, penetrationDepth);
						return true;
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
			outContacts.emplace_back(startContactPoint, endContactPoint, collisionNormal, penetrationDepth);
			return true;
		}
	}
};