#pragma once

#include "src/ECS/System.h"

#include "src/Components/RigidbodyComponent.h"

#include "src/Physics/PhysicsEngine.h"
#include "src/Physics/Constants.h"
#include "../Games/GalaxyGolf/WorldSettings.h"

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
		// Collision Resolution. Since this method is deprecated, using only the first contact to resolve collision
		PhysicsEngine::ResolveCollision(
			event.contacts[0].startContactPoint,
			event.contacts[0].endContactPoint,
			event.contacts[0].penetrationDepth,
			event.contacts[0].collisionNormal,
			event.a.GetComponent<RigidBodyComponent>(),
			event.b.GetComponent<RigidBodyComponent>(),
			event.a.GetComponent<TransformComponent>(),
			event.b.GetComponent<TransformComponent>()
		);
	}

	// Add and integrate forces for non-kinematic bodies
	void UpdateForces(const float deltaTime, const WorldSettings& worldSettings)
	{
		for (auto entity : GetSystemEntities())
		{
			// auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

			if (!rigidBody.isKinematic)
			{
				// Adding weight force
				Vector2 weight = Vector2(0.0f, (rigidBody.mass * worldSettings.gravity * Physics::PIXEL_PER_METER));
				rigidBody.AddForce(weight);

				// Adding wind force
				Vector2 wind = Vector2(worldSettings.windSpeed, 0.0f);
				rigidBody.AddForce(wind);

				// Adding Drag force
				Vector2 drag = PhysicsEngine::GenerateDragForce(rigidBody, worldSettings.atmosphereDrag);
				rigidBody.AddForce(drag);


				// AddSpringForceToConnectedEntities(entity);

				// HandleEdgeCollision(entity, transform, rigidBody);

				PhysicsEngine::IntegrateForces(rigidBody, deltaTime);
			}
		}
	}

	// Integrate velocity and acceleration (linear and angular) for all the bodies and update their collider
	void UpdateVelocities(const float deltaTime)
	{
		for (auto entity : GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

			if (rigidBody.isKinematic)
			{
				rigidBody.velocity += rigidBody.acceleration * deltaTime;
				transform.position += rigidBody.velocity * deltaTime;
				rigidBody.angularVelocity += rigidBody.angularAcceleration * deltaTime;
				transform.rotation += rigidBody.angularVelocity * deltaTime;
			}
			else
			{
				PhysicsEngine::IntegrateVelocities(rigidBody, transform, deltaTime);
			}
		}
		// Update collider for all the bodies
		for (auto entity : GetSystemEntities())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			// auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

			PhysicsEngine::UpdateColliderProperties(entity, transform);
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