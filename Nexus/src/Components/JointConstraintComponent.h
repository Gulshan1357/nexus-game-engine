#pragma once

#include "src/ECS/Entity.h"
#include "src/Utils/Matrix.h"
#include "src/Utils/Vector2.h"

// TODO: What happens if the entity of the joint is delete? Also delete the whole joint?

/**
 * JointConstraintComponent provides information for Constraint System. Always add it with ConstraintTypeComponent
 * @param a (Entity) First entity connected with joint entity. Entity should have a RigidBody.
 * @param b (Entity) Second entity connected with joint entity. Entity should have a RigidBody.
*/
struct JointConstraintComponent
{
	Entity a;
	Entity b;

	// Values populated by ConstraintSystem
	Vector2 anchorPointForA;	// The location of anchor point w.r.t to A's local space. Calculated by ConstraintSystem::InitializeLocalCoordinates()
	Vector2 anchorPointForB;	// The location of anchor point w.r.t to B's local space. Calculated by ConstraintSystem::InitializeLocalCoordinates()
	Matrix jacobian;			// Calculated by ConstraintSystem::PreSolve()
	VectorN cachedLambda;		// Calculated by ConstraintSystem::Solve()
	float bias = 0.0f;			// Baumgarte stabilization factor calculated by ConstraintSystem::PreSolve()

	explicit JointConstraintComponent(const Entity a, const Entity b)
		: a(a), b(b), jacobian(1, 6), cachedLambda(1)
	{
		jacobian.Zero();
		cachedLambda.Zero();
	}
};