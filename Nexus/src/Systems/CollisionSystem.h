#pragma once

#include "../ECS/System.h"
#include "../ECS/Entity.h"

#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"

#include "../Event/EventBus.h"
#include "../Events/CollisionEvent.h"

#include "../Utils/Logger.h"

class CollisionSystem : public System
{
public:
	CollisionSystem()
	{
		RequireComponent<BoxColliderComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(const std::unique_ptr<EventBus>& eventBus)
	{
		auto entities = GetSystemEntities();

		// Loop all entities that the system is interested in
		for (auto i = entities.begin(); i != entities.end(); ++i)
		{
			Entity a = *i;
			const auto aTransform = a.GetComponent<TransformComponent>();
			const auto aCollider = a.GetComponent<BoxColliderComponent>();

			// Loop all the entities that still need to be checked
			for (auto j = i; j != entities.end(); ++j)
			{
				Entity b = *j;

				// Bypass if we are trying to test the same entity
				if (a == b)
				{
					continue;
				}

				const auto bTransform = b.GetComponent<TransformComponent>();
				const auto bCollider = b.GetComponent<BoxColliderComponent>();

				// Perform AABB collision check
				bool collisionHappened = CheckAABBCollision(
					aTransform.position.x + aCollider.offset.x,
					aTransform.position.y + aCollider.offset.y,
					aCollider.width,
					aCollider.height,
					bTransform.position.x + bCollider.offset.x,
					bTransform.position.y + bCollider.offset.y,
					bCollider.width,
					bCollider.height
				);

				if (collisionHappened)
				{
					// Logger::Log("Entity " + std::to_string(a.GetId()) + " is colliding with entity " + std::to_string(b.GetId()));

					// TODO: emit an event....
					/*if (a.BelongsToGroup("Player1") && b.BelongsToGroup("Player2"))
					{
						Logger::Log("Entity Player1 is colliding with entity Player2");

					}*/
					eventBus->EmitEvent<CollisionEvent>(a, b);
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