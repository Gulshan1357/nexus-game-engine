#pragma once

#include <memory>
#include <vector>

#include "src/ECS/System.h"

#include "src/Physics/PenetrationConstraint.h"


class CollisionEvent;
class EventManager;
struct ConstraintTypeComponent;

class ConstraintSystem : public System
{
public:
	ConstraintSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<ConstraintTypeComponent>();
	}

	// Calculate the local coordinates of the joint anchorPoint w.r.t the center of mass of the entity 'a' and 'b'
	void InitializeLocalCoordinates() const;

	// To populate the m_penetrations vector
	void SubscribeToEvents(const std::unique_ptr<EventManager>& eventManager);
	void onCollision(const CollisionEvent& event);

	// Execute all the steps (PreSolve, Solve and PostSolve) to resolve constrains
	void Update(const float deltaTime);

	// Pre-solving step: Calculate Jacobian and apply cached impulses (lambda)
	void PreSolve(const float deltaTime);
	// Solve step: Resolve constraints and accumulate impulses
	void Solve();


	// For Penetration constraint

	// Pre-solving step: Calculate Jacobian and apply cached impulses (lambda)
	void PreSolvePenetration(const float deltaTime);
	// Solve step: Resolve constraints and accumulate impulses
	void SolvePenetration();

	// Manage penetration vector. Whenever a collision happens a new penetration is added to the vector and after resolution they are cleared.
	std::vector<PenetrationConstraint>& GetPenetrations();
	void ClearPenetrations();
	void AddPenetration(Entity a, Entity b, Vector2 startContactPoint, Vector2 endContactPoint, Vector2 collisionNormal);
	size_t GetPenetrationSize() const;

private:
	std::vector<PenetrationConstraint> m_penetrations;
};
