#pragma once

#include "src/ECS/System.h"

#include "src/Components/RigidbodyComponent.h"
#include "src/Components/TransformComponent.h"
#include "src/EventManagement/EventManager.h"
#include "src/Physics/PhysicsEngine.h"
#include "src/Physics/Constants.h"
#include "../Games/GalaxyGolf/WorldSettings.h"
#include "src/Events/CollisionEvent.h"
#include "src/Utils/Random.h"
#include "src/Utils/Logger.h"

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
		for (auto& entity : GetSystemEntities())
		{
			// auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

			if (!entity.BelongsToGroup("Aliens")) // No gravity for bodies that are getting attracted by gravitational force b/w each other
			{
				// Adding weight force
				Vector2 weight = Vector2(0.0f, (rigidBody.mass * worldSettings.gravity * Physics::PIXEL_PER_METER));
				rigidBody.AddForce(weight);
			}

			// Adding wind force
			Vector2 wind = Vector2(worldSettings.windSpeed, 0.0f);
			rigidBody.AddForce(wind);

			// Adding Drag force
			Vector2 drag = PhysicsEngine::GenerateDragForce(rigidBody, worldSettings.atmosphereDrag);
			rigidBody.AddForce(drag);


			AddSpringForceToConnectedEntities(entity);


			if (entity.BelongsToGroup("Aliens"))
			{
				for (auto& ball : entity.GetEntitiesByRelationshipTag("Attracted"))
				{
					rigidBody.AddForce(PhysicsEngine::GenerateGravitationalForce(
						rigidBody,
						ball.GetComponent<RigidBodyComponent>(),
						ball.GetComponent<TransformComponent>().position - entity.GetComponent<TransformComponent>().position,
						10000,
						50,
						500
					));

				}
			}

			PhysicsEngine::IntegrateForces(rigidBody, deltaTime);
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

				constexpr float springForceStrength = 150.f;
				const float restLength = Random::Float(100.f, 300.f);

				// Adding spring force
				Vector2 springForce = PhysicsEngine::GenerateSpringForce(connectedEntityTransform, transform, restLength, springForceStrength);
				connectedEntityRigidBody.AddForce(springForce);
				rigidBody.AddForce(-springForce);
			}
		}
		if (entity.BelongsToGroup("TightAnchor") || entity.BelongsToGroup("Spring"))
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

			// Adding forces to connected spring entities
			for (auto connectedEntity : entity.GetEntitiesByRelationshipTag("Spring"))
			{
				auto& connectedEntityTransform = connectedEntity.GetComponent<TransformComponent>();
				auto& connectedEntityRigidBody = connectedEntity.GetComponent<RigidBodyComponent>();

				constexpr float springForceStrength = 150.f;
				constexpr float restLength = 50.f;

				// Adding spring force
				Vector2 springForce = PhysicsEngine::GenerateSpringForce(connectedEntityTransform, transform, restLength, springForceStrength);
				connectedEntityRigidBody.AddForce(springForce);
				rigidBody.AddForce(-springForce);
			}
		}
	}
};
