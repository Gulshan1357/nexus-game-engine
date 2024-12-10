#pragma once

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/BoxColliderComponent.h"

#include "src/EventManagement/EventManager.h"
#include "src/Events/CollisionEvent.h"

#include "src/Utils/Logger.h"

class CollisionSystem : public System
{
public:
	CollisionSystem()
	{
		RequireComponent<BoxColliderComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(const std::unique_ptr<EventManager>& eventManager)
	{
		auto entities = GetSystemEntities();

		// Loop all entities that the system is interested in
		for (auto i = entities.begin(); i != entities.end(); ++i)
		{
			Entity a = *i;
			const auto& aTransform = a.GetComponent<TransformComponent>();
			const auto& aCollider = a.GetComponent<BoxColliderComponent>();

			// Calculate the bottom-left position of entity 'a' based on its position and collider-offset
			float aBottomLeftX = aTransform.position.x + aCollider.offset.x - static_cast<float>(aCollider.width) / 2.0f;
			float aBottomLeftY = aTransform.position.y + aCollider.offset.y - static_cast<float>(aCollider.height) / 2.0f;

			// Loop all the entities that still need to be checked
			for (auto j = i + 1; j != entities.end(); ++j)
			{
				Entity b = *j;

				const auto& bTransform = b.GetComponent<TransformComponent>();
				const auto& bCollider = b.GetComponent<BoxColliderComponent>();

				// Calculate the bottom-left position of entity 'b'
				float bBottomLeftX = bTransform.position.x + bCollider.offset.x - static_cast<float>(bCollider.width) / 2.0f;
				float bBottomLeftY = bTransform.position.y + bCollider.offset.y - static_cast<float>(bCollider.height) / 2.0f;

				// Perform AABB collision check
				bool collisionHappened = CheckAABBCollision(
					aBottomLeftX, aBottomLeftY, aCollider.width, aCollider.height,
					bBottomLeftX, bBottomLeftY, bCollider.width, bCollider.height
				);

				if (collisionHappened)
				{
					Logger::Log("Entity " + std::to_string(a.GetId()) + " is colliding with entity " + std::to_string(b.GetId()));
					eventManager->EmitEvent<CollisionEvent>(a, b);
				}
			}
		}
	}

	// Axis-Aligned Bounding Box Collision Detection
	static bool CheckAABBCollision(const double aX, const double aY, const double aW, const double aH, const double bX, const double bY, const double bW, const double bH)
	{
		return (
			aX < bX + bW &&
			aX + aW > bX &&
			aY < bY + bH &&
			aY + aH > bY
			);
	}

};