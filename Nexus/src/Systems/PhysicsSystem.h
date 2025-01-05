#pragma once

#include "src/ECS/System.h"

#include "src/Components/RigidbodyComponent.h"

#include "src/Physics/PhysicsEngine.h"
#include "src/Physics/Constants.h"

class PhysicsSystem : public System
{
public:
	PhysicsSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void InitializeEntityPhysics() const
	{
		Logger::Log("Calculating inverse mass, angular mass etc. for all the suitable entities");
		for (auto entity : GetSystemEntities())
		{
			PhysicsEngine::InitializeEntityPhysics(entity);
		}
	}

	void SubscribeToEvents(const std::unique_ptr<EventManager>& eventManager)
	{
		using CallbackType = std::function<void(PhysicsSystem*, CollisionEvent&)>;
		const CallbackType callback = [this](auto&&, auto&& placeholder2) { OnCollision(std::forward<decltype(placeholder2)>(placeholder2)); };
		eventManager->SubscribeToEvent<CollisionEvent>(this, callback);
		//  std::placeholders::_2 tells std::bind that the second argument (the event) will be provided when the resulting function is called. This allows us to create a callable object that matches the required function signature of SubscribeToEvent, where the first argument is the instance (PhysicsSystem*), and the second argument is the event (CollisionEvent&).
	}

	static void OnCollision(const CollisionEvent& event)
	{
		// Collision Resolution
		PhysicsEngine::ResolveCollision(
			event.contact.startContactPoint,
			event.contact.endContactPoint,
			event.contact.penetrationDepth,
			event.contact.collisionNormal,
			event.a.GetComponent<RigidBodyComponent>(),
			event.b.GetComponent<RigidBodyComponent>(),
			event.a.GetComponent<TransformComponent>(),
			event.b.GetComponent<TransformComponent>()
		);
	}

	void Update(const float deltaTime)
	{
		// Delta time is in milliseconds so dividing by 1000 to convert it into seconds
		const float dt = deltaTime / 1000;

		for (auto entity : GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

			if (rigidBody.isKinematic)
			{
				rigidBody.velocity += rigidBody.acceleration * dt;
				transform.position += rigidBody.velocity * dt;
				PhysicsEngine::UpdateColliderProperties(entity, transform);
			}
			else
			{
				// Integrating all the forces and torque acting on the RigidBody using Physics Engine
				// transform.position += PhysicsEngine::IntegrateLinear(rigidBody, dt);
				// transform.rotation += PhysicsEngine::IntegrateAngular(rigidBody, dt);
				PhysicsEngine::IntegrateForces(rigidBody, dt);
				PhysicsEngine::IntegrateVelocities(rigidBody, transform, dt);
				PhysicsEngine::UpdateColliderProperties(entity, transform);

				//------------------------------------------------------------------------
				// Apply constantly acting forces like gravity, drag etc.
				//------------------------------------------------------------------------

				// Adding Drag force
				Vector2 drag = PhysicsEngine::GenerateDragForce(rigidBody, 0.01f);
				rigidBody.AddForce(drag);

				// Adding weight force
				Vector2 weight = Vector2(0.0f, rigidBody.mass * -Physics::gravity * Physics::PIXEL_PER_METER);
				rigidBody.AddForce(weight);

				// AddSpringForceToConnectedEntities(entity);

				// HandleEdgeCollision(entity, transform, rigidBody);
			}
		}
	}

	static void AddSpringForceToConnectedEntities(const Entity& entity)
	{
		if (entity.BelongsToGroup("Anchor") || entity.BelongsToGroup("Spring"))
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

			// Adding forces to connected spring entities
			for (auto connectedEntity : entity.GetEntitiesByRelationshipTag("Spring"))
			{
				auto& connectedEntityTransform = connectedEntity.GetComponent<TransformComponent>();
				auto& connectedEntityRigidBody = connectedEntity.GetComponent<RigidBodyComponent>();

				// // Adding spring force
				Vector2 springForce = PhysicsEngine::GenerateSpringForce(connectedEntityTransform, transform, 200, 1500);
				connectedEntityRigidBody.AddForce(springForce);
				rigidBody.AddForce(-springForce);
			}
		}
	}
};