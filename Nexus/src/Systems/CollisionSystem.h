#pragma once

#include <memory>

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"


#include "src/Physics/Contact.h"


class EventManager;
struct TransformComponent;
struct ColliderTypeComponent;

class CollisionSystem : public System
{
public:
	CollisionSystem()
	{
		RequireComponent<ColliderTypeComponent>();
		RequireComponent<TransformComponent>();
	}

	[[nodiscard]] static bool ShouldIgnoreCollision(Entity a, Entity b);

	void Update(const std::shared_ptr<EventManager>& eventManager) const;

	static bool IsColliding(const Entity a, const Entity b, const ColliderTypeComponent& aType, const ColliderTypeComponent& bType, std::vector<Contact>& contacts);

	// Collision detection between circle-circle
	static bool IsCollidingCircleCircle(const Entity a, const Entity b, std::vector<Contact>& outContacts);

	// Collision detection between box-box, polygon-polygon and box-polygon
	static bool IsCollidingPolygonPolygon(const Entity a, const Entity b, std::vector<Contact>& outContacts);

	// Collision detection between circle-polygon(or box)
	static bool IsCollidingCirclePolygon(const Entity circleEntity, const Entity polygonEntity, std::vector<Contact>& outContacts);
};
