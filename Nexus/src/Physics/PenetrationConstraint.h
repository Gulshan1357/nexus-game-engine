#pragma once

#include "src/ECS/Entity.h"
#include "src/Physics/PhysicsEngine.h"
#include "src/Utils/Matrix.h"
#include "src/Utils/Vector2.h"

/**
 * Warning: Do not add this component to an entity. This componnet is
 * PenetrationConstraint provides information for Constraint System.
 * @param a (Entity) First entity in the collision.
 * @param b (Entity) Second entity in the collision.
 * @param aCollisionPoint (Vector2) The location of the collision point in world space.
 * @param bCollisionPoint (Vector2) The location of the collision point in world space.
 * @param collisionNormal (Vector2) The collision normal vector in world space.
*/
class PenetrationConstraint
{
public:
	Entity a;
	Entity b;
	Vector2 aCollisionPoint;	// The location of the collision point w.r.t to A's local space.
	Vector2 bCollisionPoint;	// The location of the collision point w.r.t to B's local space.
	Vector2 collisionNormal;	// The collision normal vector w.r.t to A's local space.

	// Values populated by ConstraintSystem
	Matrix jacobian;			// Calculated by ConstraintSystem::PreSolve()
	VectorN cachedLambda;		// Calculated by ConstraintSystem::Solve()
	float bias;					// Baumgarte stabilization factor calculated by ConstraintSystem::PreSolve()
	float friction;				// Friction coefficient between the two penetrating bodies

	// PenetrationConstraint() : m_entityA(), m_entityB(), jacobian(1, 6), cachedLambda(1), bias(0.0f)
	// {}

	explicit PenetrationConstraint(
		Entity a,
		Entity b,
		const Vector2& aCollisionPoint,
		const Vector2& bCollisionPoint,
		const Vector2& collisionNormal)
		: a(a), b(b), jacobian(2, 6), cachedLambda(2), bias(0.0f), friction(0.0f)
	{
		this->aCollisionPoint = PhysicsEngine::WorldSpaceToLocalSpace(a.GetComponent<TransformComponent>(), aCollisionPoint);
		this->bCollisionPoint = PhysicsEngine::WorldSpaceToLocalSpace(b.GetComponent<TransformComponent>(), bCollisionPoint);
		this->collisionNormal = PhysicsEngine::WorldSpaceToLocalSpace(a.GetComponent<TransformComponent>(), collisionNormal);
		jacobian.Zero();
		cachedLambda.Zero();
	}
};
