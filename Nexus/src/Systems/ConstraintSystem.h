#pragma once

#include "src/ECS/System.h"

#include "src/Components/ConstraintTypeComponent.h"
#include "src/Components/JointConstraintComponent.h"
#include "src/Components/RigidBodyComponent.h"

#include "src/Utils/Matrix.h"

class ConstraintSystem : public System
{
public:
	ConstraintSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<ConstraintTypeComponent>();
	}

	// Calculate the local coordinates of the joint anchorPoint w.r.t the center of mass of the entity 'a' and 'b'
	void InitializeLocalCoordinates() const
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& anchorTransform = entity.GetComponent<TransformComponent>();
			const auto& constraintType = entity.GetComponent<ConstraintTypeComponent>();

			if (constraintType.type == ConstrainType::JOINT)
			{
				auto& jointComponent = entity.GetComponent<JointConstraintComponent>();
				auto& aTransform = jointComponent.a.GetComponent<TransformComponent>();
				auto& bTransform = jointComponent.b.GetComponent<TransformComponent>();
				jointComponent.anchorPointForA = PhysicsEngine::WorldSpaceToLocalSpace(aTransform, anchorTransform.position);
				jointComponent.anchorPointForB = PhysicsEngine::WorldSpaceToLocalSpace(bTransform, anchorTransform.position);
			}
		}
	}

	// Pre-solving step: Calculate Jacobian and apply cached impulses (lambda)
	void PreSolve(const float deltaTime) const
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& constraintType = entity.GetComponent<ConstraintTypeComponent>();

			// if (!rigidBody.isKinematic)
			// {
			// }

			if (constraintType.type != ConstrainType::JOINT)
				continue;

			auto& jointComponent = entity.GetComponent<JointConstraintComponent>();
			const auto& entityA = jointComponent.a;
			const auto& entityB = jointComponent.b;

			// The connected entities should have rigidbody component so that we can use the inverse mass for constraint
			if (!entityA.HasComponent<RigidBodyComponent>() || !entityB.HasComponent<RigidBodyComponent>())
				continue;

			auto& transformA = entityA.GetComponent<TransformComponent>();
			auto& transformB = entityB.GetComponent<TransformComponent>();
			auto& rigidbodyA = entityA.GetComponent<RigidBodyComponent>();
			auto& rigidbodyB = entityB.GetComponent<RigidBodyComponent>();

			// Anchor point position in the world space
			const Vector2 anchorAWorld = PhysicsEngine::LocalSpaceToWorldSpace(transformA, jointComponent.anchorPointForA);
			const Vector2 anchorBWorld = PhysicsEngine::LocalSpaceToWorldSpace(transformB, jointComponent.anchorPointForB);

			const Vector2 rA = anchorAWorld - transformA.position; // Distance between the anchor point and the center of mass of the entity
			const Vector2 rB = anchorBWorld - transformB.position; // Distance between the anchor point and the center of mass of the entity

			CalculateJacobianRows(jointComponent, anchorAWorld, anchorBWorld, rA, rB);
			const Matrix jacobianTranspose = jointComponent.jacobian.Transpose();

			// Warm-starting: Apply cached impulses
			VectorN cachedImpulses = jacobianTranspose * jointComponent.cachedLambda;

			// Since impulses(lambda) = 
			//  [ jacobianLinearA.x ]
			//  [ jacobianLinearA.y ]
			//  [ jacobianAngularA   ]
			//  [ jacobianLinearB.x ]
			//  [ jacobianLinearB.y ]
			//  [ jacobianAngularB  ]
			// Apply impulses to bodies
			if (!rigidbodyA.isKinematic)
			{
				rigidbodyA.ApplyImpulseLinear(Vector2(cachedImpulses[0], cachedImpulses[1]));	// A linear impulse
				rigidbodyA.ApplyImpulseAngular(cachedImpulses[2]);										// A angular impulse

			}
			if (!rigidbodyB.isKinematic)
			{
				rigidbodyB.ApplyImpulseLinear(Vector2(cachedImpulses[3], cachedImpulses[4]));	// B linear impulse
				rigidbodyB.ApplyImpulseAngular(cachedImpulses[5]);										// B angular impulse
			}

			// Baumgarte stabilization(bias) for position error
			constexpr float positionErrorThreshold = 0.01f;
			constexpr float beta = 0.2f;
			float positionError = (anchorBWorld - anchorAWorld).Dot(anchorBWorld - anchorAWorld);
			if (positionError > positionErrorThreshold) positionError -= positionErrorThreshold; // To make sure that the positionalError is within limits
			else 	positionError = 0.0f;
			jointComponent.bias = (beta / deltaTime) * positionError;
		}
	}

	// Solve step: Resolve constraints and accumulate impulses
	void Solve() const
	{
		// TODO: resolve constrains for non-kinematic bodies
		for (auto entity : GetSystemEntities())
		{
			const auto& constraintType = entity.GetComponent<ConstraintTypeComponent>();

			// if (!rigidBody.isKinematic)
			// {
			// }

			if (constraintType.type != ConstrainType::JOINT)
				continue;

			auto& jointComponent = entity.GetComponent<JointConstraintComponent>();
			const auto& entityA = jointComponent.a;
			const auto& entityB = jointComponent.b;

			// The connected entities should have rigidbody component so that we can use the inverse mass for constraint
			if (!entityA.HasComponent<RigidBodyComponent>() || !entityB.HasComponent<RigidBodyComponent>())
				continue;

			auto& rigidbodyA = entityA.GetComponent<RigidBodyComponent>();
			auto& rigidbodyB = entityB.GetComponent<RigidBodyComponent>();

			// Calculate velocities and inverse mass matrix
			const VectorN velocities = PhysicsEngine::GetVelocitiesVector(rigidbodyA, rigidbodyB);
			const Matrix inverseMassMatrix = PhysicsEngine::GetInverseMassMatrix(rigidbodyA, rigidbodyB);

			// Calculate lambda (constraint impulses)
			// lambda = -(J V + b) / (J M^-1 JT) or,
			// lambda = -(Jacobian Matrix * VelocitiesVector + bias) / (Jacobian Matrix * InverseMass Matrix * Jacobian Matrix transpose)

			VectorN numerator = jointComponent.jacobian * velocities * -1.0f;
			numerator[0] -= jointComponent.bias;

			const Matrix jacobianTranspose = jointComponent.jacobian.Transpose();
			Matrix denominator = jointComponent.jacobian * inverseMassMatrix * jacobianTranspose;

			// lambda = num/deno => lambda * denominator = numerator.
			// Since this is of the form Ax = B solving using Gauss-Seidel method
			VectorN lambda = Matrix::SolveGaussSeidel(denominator, numerator);

			jointComponent.cachedLambda += lambda;

			// Computing impulses with direction and magnitude
			VectorN impulses = jacobianTranspose * lambda;

			// Applying impulses to both A and B
			if (!rigidbodyA.isKinematic)
			{
				rigidbodyA.ApplyImpulseLinear(Vector2(impulses[0], impulses[1])); // A linear impulse
				rigidbodyA.ApplyImpulseAngular(impulses[2]);								// A angular impulse

			}
			if (!rigidbodyB.isKinematic)
			{
				rigidbodyB.ApplyImpulseLinear(Vector2(impulses[3], impulses[4])); // B linear impulse
				rigidbodyB.ApplyImpulseAngular(impulses[5]);								// B angular impulse
			}
			// Logger::Warn("Impulse Linear A: " + Vector2(impulses[0], impulses[1]).ToString());
			// Logger::Warn("Impulse Angular A: " + std::to_string(impulses[2]));
			// Logger::Warn("Impulse Linear B: " + Vector2(impulses[3], impulses[4]).ToString());
			// Logger::Warn("Impulse Angular B: " + std::to_string(impulses[5]));

		}
	}

	// Post-solve step: 
	void PostSolve() const
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& constraintType = entity.GetComponent<ConstraintTypeComponent>();

			// if (!rigidBody.isKinematic)
			// {
			// }

			if (constraintType.type == ConstrainType::JOINT)
			{
				auto& jointComponent = entity.GetComponent<JointConstraintComponent>();
				const auto& entityA = jointComponent.a;
				const auto& entityB = jointComponent.b;

				// The connected entities should have rigidbody component so that we can use the inverse mass for constraint
				if (entityA.HasComponent<RigidBodyComponent>() && entityB.HasComponent<RigidBodyComponent>())
				{
					auto& transformA = entityA.GetComponent<TransformComponent>();
					auto& transformB = entityB.GetComponent<TransformComponent>();
					// auto& rigidbodyA = entityA.GetComponent<RigidBodyComponent>();
					// auto& rigidbodyB = entityB.GetComponent<RigidBodyComponent>();

					// TODO:....

					// Update the anchor transform. The anchor should be exactly in the middle between the transformA and transformB
					auto& transform = entity.GetComponent<TransformComponent>();
					transform.position = (transformA.position + transformB.position) * 0.5f;
				}
			}
		}
	}

private:
	// Helper function to calculate Jacobian matrix
	static void CalculateJacobianRows(JointConstraintComponent& joint, const Vector2& anchorAWorld, const Vector2& anchorBWorld, const Vector2& rA, const Vector2& rB)
	{
		// First derivative of constrain = Jacobian matrix * velocity vector
		// Where, Jacobian Matrix = [2(ra-rb), 2( ra(vector) x (ra - rb) ), 2(rb-ra), 2( rb(vector) x (rb - ra) )]
		// And, velocity vector = PhysicsEngine::GetVelocitiesVector()

		joint.jacobian.Zero();

		const Vector2 linearA = (anchorAWorld - anchorBWorld) * 2.0f;
		joint.jacobian.rowVectors[0][0] = linearA.x;
		joint.jacobian.rowVectors[0][1] = linearA.y;
		joint.jacobian.rowVectors[0][2] = rA.Cross(anchorAWorld - anchorBWorld) * 2.0f; // Angular A

		const Vector2 linearB = (anchorBWorld - anchorAWorld) * 2.0f;
		joint.jacobian.rowVectors[0][3] = linearB.x;
		joint.jacobian.rowVectors[0][4] = linearB.y;
		joint.jacobian.rowVectors[0][5] = rB.Cross(anchorBWorld - anchorAWorld) * 2.0f;  // Angular B
	}
};
