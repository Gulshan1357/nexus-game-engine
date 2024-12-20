#pragma once

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/ColliderTypeComponent.h"

#include "src/EventManagement/EventManager.h"
#include "src/Events/CollisionEvent.h"

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

	static void HandleCollision(const std::unique_ptr<EventManager>& eventManager, Entity a, Entity b,
		ColliderType aType, ColliderType bType)
	{
		if (aType == ColliderType::Box && bType == ColliderType::Box)
		{
			HandleBoxBoxCollision(eventManager, a, b);
		}
	}

	static void HandleBoxBoxCollision(const std::unique_ptr<EventManager>& eventManager, Entity a, Entity b)
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

		// Check for Axis-Aligned Bounding Box collision
		if (CheckAABBCollision(
			aBottomLeftX, aBottomLeftY, aBoxCollider.width, aBoxCollider.height,
			bBottomLeftX, bBottomLeftY, bBoxCollider.width, bBoxCollider.height
		))
		{
			Logger::Log("Entity " + std::to_string(a.GetId()) + " is colliding with entity " + std::to_string(b.GetId()));
			eventManager->EmitEvent<CollisionEvent>(a, b);
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