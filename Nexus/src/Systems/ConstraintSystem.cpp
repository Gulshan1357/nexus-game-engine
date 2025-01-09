#include "stdafx.h"

#include "ConstraintSystem.h"

#include "src/ECS/Entity.h"
#include "src/ECS/Coordinator.h"
#include "src/EventManagement/EventManager.h"
#include "src/Events/CollisionEvent.h"

#include "src/Components/RigidBodyComponent.h"
#include "src/Components/TransformComponent.h"
#include "src/Components/JointConstraintComponent.h"
#include "src/Components/ConstraintTypeComponent.h"
#include "src/Physics/PenetrationConstraint.h"

void ConstraintSystem::InitializeLocalCoordinates() const
{
	for (auto& entity : GetSystemEntities())
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

void ConstraintSystem::SubscribeToEvents(const std::unique_ptr<EventManager>& eventManager)
{
	// // Clean up all dynamically allocated objects
	using CallbackType = std::function<void(ConstraintSystem*, CollisionEvent&)>;
	const CallbackType callback = [this](auto&&, auto&& placeholder2) { onCollision(std::forward<decltype(placeholder2)>(placeholder2)); };
	eventManager->SubscribeToEvent<CollisionEvent>(this, callback);
	//  std::placeholders::_2 tells std::bind that the second argument (the event) will be provided when the resulting function is called. This allows us to create a callable object that matches the required function signature of SubscribeToEvent, where the first argument is the instance (DamageSystem*), and the second argument is the event (CollisionEvent&).
}

void ConstraintSystem::onCollision(CollisionEvent& event)
{
	AddPenetration(
		event.a,
		event.b,
		event.contact.startContactPoint,
		event.contact.endContactPoint,
		event.contact.collisionNormal
	);
	// Logger::Warn("OnCollision GetPenetrationSize: " + std::to_string(GetPenetrationSize()));
}


void ConstraintSystem::Update(const float deltaTime)
{
	// Logger::Log("ConstraintSystem::Update GetPenetrationSize: " + std::to_string(GetPenetrationSize()));
	PreSolve(deltaTime);
	PreSolvePenetration(deltaTime);
	// Multiple iteration for better accuracy
	for (int i = 0; i < 5; i++)
	{
		Solve();
		SolvePenetration();
	}
	PostSolve();
	PostSolvePenetration();
	ClearPenetrations();
}


void ConstraintSystem::PreSolve(const float deltaTime)
{
	for (const auto& entity : GetSystemEntities())
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


		// First derivative of constrain = Jacobian matrix * velocity vector
		// Where, Jacobian Matrix = [2(ra-rb), 2( ra(vector) x (ra - rb) ), 2(rb-ra), 2( rb(vector) x (rb - ra) )]
		// And, velocity vector = PhysicsEngine::GetVelocitiesVector()
		jointComponent.jacobian.Zero();

		const Vector2 linearA = (anchorAWorld - anchorBWorld) * 2.0f;
		jointComponent.jacobian.rowVectors[0][0] = linearA.x;
		jointComponent.jacobian.rowVectors[0][1] = linearA.y;
		jointComponent.jacobian.rowVectors[0][2] = rA.Cross(anchorAWorld - anchorBWorld) * 2.0f; // Angular A

		const Vector2 linearB = (anchorBWorld - anchorAWorld) * 2.0f;
		jointComponent.jacobian.rowVectors[0][3] = linearB.x;
		jointComponent.jacobian.rowVectors[0][4] = linearB.y;
		jointComponent.jacobian.rowVectors[0][5] = rB.Cross(anchorBWorld - anchorAWorld) * 2.0f;  // Angular B

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
		positionError = std::max(0.0f, positionError - positionErrorThreshold); // To make sure that the positionalError is within limits
		jointComponent.bias = (beta / deltaTime) * positionError;
	}
}

void ConstraintSystem::Solve()
{
	// TODO: resolve constrains for non-kinematic bodies
	for (const auto& entity : GetSystemEntities())
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

void ConstraintSystem::PostSolve()
{
	for (const auto& entity : GetSystemEntities())
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

void ConstraintSystem::PreSolvePenetration(const float deltaTime)
{

	for (auto& penetration : m_penetrations)
	{
		const Entity& entityA = penetration.a;
		const Entity& entityB = penetration.b;

		// The connected entities should have rigidbody component so that we can use the inverse mass for constraint
		if (!entityA.HasComponent<RigidBodyComponent>() || !entityB.HasComponent<RigidBodyComponent>())
			continue;

		auto& transformA = entityA.GetComponent<TransformComponent>();
		auto& transformB = entityB.GetComponent<TransformComponent>();
		auto& rigidbodyA = entityA.GetComponent<RigidBodyComponent>();
		auto& rigidbodyB = entityB.GetComponent<RigidBodyComponent>();

		// Collision point position in the world space
		const Vector2 collisionAWorld = PhysicsEngine::LocalSpaceToWorldSpace(transformA, penetration.aCollisionPoint);
		const Vector2 collisionBWorld = PhysicsEngine::LocalSpaceToWorldSpace(transformB, penetration.bCollisionPoint);
		Vector2 normalWorld = PhysicsEngine::LocalSpaceToWorldSpace(transformA, penetration.collisionNormal);

		const Vector2 rA = collisionAWorld - transformA.position; // Distance between the anchor point and the center of mass of the entity
		const Vector2 rB = collisionBWorld - transformB.position; // Distance between the anchor point and the center of mass of the entity

		// First derivative of constrain = Jacobian matrix * velocity vector
		// Where, Jacobian Matrix = [-normal, -2( ra(vector) x normal ), normal, 2( rb(vector) x normal )]
		// And, velocity vector = PhysicsEngine::GetVelocitiesVector()
		penetration.jacobian.Zero();

		const Vector2 linearA = -normalWorld;
		penetration.jacobian.rowVectors[0][0] = linearA.x;
		penetration.jacobian.rowVectors[0][1] = linearA.y;
		penetration.jacobian.rowVectors[0][2] = -rA.Cross(normalWorld); // Angular A

		const Vector2 linearB = normalWorld;
		penetration.jacobian.rowVectors[0][3] = linearB.x;
		penetration.jacobian.rowVectors[0][4] = linearB.y;
		penetration.jacobian.rowVectors[0][5] = rB.Cross(normalWorld);  // Angular B

		const Matrix jacobianTranspose = penetration.jacobian.Transpose();

		// Warm-starting: Apply cached impulses
		VectorN cachedImpulses = jacobianTranspose * penetration.cachedLambda;

		// Since impulses(lambda) = 
		//  [ jacobianLinearA  ]
		//  [ jacobianAngularA ]
		//  [ jacobianLinearB  ]
		//  [ jacobianAngularB ]
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
		float positionCorrection = (collisionBWorld - collisionAWorld).Dot(-normalWorld);
		positionCorrection = std::min(0.0f, positionCorrection + positionErrorThreshold);
		penetration.bias = (beta / deltaTime) * positionCorrection;
	}
}

void ConstraintSystem::SolvePenetration()
{
	// TODO: resolve constrains for non-kinematic bodies
	for (auto& penetration : m_penetrations)
	{
		const Entity& entityA = penetration.a;
		const Entity& entityB = penetration.b;

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

		VectorN numerator = penetration.jacobian * velocities * -1.0f;
		numerator[0] -= penetration.bias;

		const Matrix jacobianTranspose = penetration.jacobian.Transpose();
		Matrix denominator = penetration.jacobian * inverseMassMatrix * jacobianTranspose;

		// lambda = num/deno => lambda * denominator = numerator.
		// Since this is of the form Ax = B solving using Gauss-Seidel method
		VectorN lambda = Matrix::SolveGaussSeidel(denominator, numerator);

		// Accumulate impulses and clamp it within constraint limits. The cachedLambda will be used by PreSolve
		VectorN oldLambda = penetration.cachedLambda;
		penetration.cachedLambda += lambda;
		penetration.cachedLambda[0] = (penetration.cachedLambda[0] < 0.0f) ? 0.0f : penetration.cachedLambda[0];
		lambda = penetration.cachedLambda - oldLambda;

		// Computing impulses with direction and magnitude
		VectorN impulses = jacobianTranspose * lambda;

		// Applying impulses to both A and B
		if (!rigidbodyA.isKinematic)
		{
			rigidbodyA.ApplyImpulseLinear(Vector2(impulses[0], impulses[1])); // A linear impulse
			rigidbodyA.ApplyImpulseAngular(impulses[2]);								// A angular impulse
			// Logger::Log("A impulse: " + Vector2(impulses[0], impulses[1]).ToString());
		}
		if (!rigidbodyB.isKinematic)
		{
			rigidbodyB.ApplyImpulseLinear(Vector2(impulses[3], impulses[4])); // B linear impulse
			rigidbodyB.ApplyImpulseAngular(impulses[5]);								// B angular impulse
			// Logger::Log("B impulse: " + Vector2(impulses[3], impulses[4]).ToString());
		}
	}
}

void ConstraintSystem::PostSolvePenetration()
{
	for (auto& penetration : m_penetrations)
	{
		const Entity& entityA = penetration.a;
		const Entity& entityB = penetration.b;

		// The connected entities should have rigidbody component so that we can use the inverse mass for constraint
		if (entityA.HasComponent<RigidBodyComponent>() && entityB.HasComponent<RigidBodyComponent>())
		{
			// auto& rigidbodyA = entityA.GetComponent<RigidBodyComponent>();
			// auto& rigidbodyB = entityB.GetComponent<RigidBodyComponent>();

			// TODO:....

		}

	}
}

std::vector<PenetrationConstraint>& ConstraintSystem::GetPenetrations()
{
	return m_penetrations;
}

void ConstraintSystem::ClearPenetrations()
{
	m_penetrations.clear();
}

void ConstraintSystem::AddPenetration(Entity a, Entity b, Vector2 startContactPoint, Vector2 endContactPoint,
	Vector2 collisionNormal)
{
	m_penetrations.emplace_back(a, b, startContactPoint, endContactPoint, collisionNormal);
}

size_t ConstraintSystem::GetPenetrationSize() const
{
	return m_penetrations.size();
}
