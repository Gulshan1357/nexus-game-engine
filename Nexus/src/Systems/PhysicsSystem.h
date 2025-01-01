#pragma once

#include "src/ECS/System.h"

#include "src/Components/ColliderTypeComponent.h"
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

			if (!rigidBody.bUsePhysics)
			{
				rigidBody.velocity += rigidBody.acceleration * dt;
				transform.position += rigidBody.velocity * dt;
			}
			else
			{
				// Integrating all the forces and torque acting on the RigidBody using Physics Engine
				transform.position += PhysicsEngine::IntegrateLinear(rigidBody, dt);
				transform.rotation += PhysicsEngine::IntegrateAngular(rigidBody, dt);

				//------------------------------------------------------------------------
				// Update entity's collider (Example circleCollider.globalCenter)
				//------------------------------------------------------------------------

				if (entity.HasComponent<BoxColliderComponent>())
				{
					UpdateBoxColliderVertices(entity.GetComponent<BoxColliderComponent>(), transform);
				}
				else if (entity.HasComponent<PolygonColliderComponent>())
				{
					UpdatePolygonColliderVertices(entity.GetComponent<PolygonColliderComponent>(), transform);
				}
				else if (entity.HasComponent<CircleColliderComponent>())
				{
					UpdateCircleColliderCenter(entity.GetComponent<CircleColliderComponent>(), transform);
				}

				//------------------------------------------------------------------------
				// Apply constantly acting forces like gravity, drag etc.
				//------------------------------------------------------------------------

				// Adding Drag force
				Vector2 drag = PhysicsEngine::GenerateDragForce(rigidBody, 0.01f);
				rigidBody.AddForce(drag);

				// Adding weight force
				Vector2 weight = Vector2(0.0f, rigidBody.mass * -9.8f * Physics::PIXEL_PER_METER);
				rigidBody.AddForce(weight);


				//------------------------------------------------------------------------
				// Apply forces to connected Entities
				//------------------------------------------------------------------------

				// if (entity.HasTag("Player2") || entity.HasTag("BigBall") || entity.BelongsToGroup("Anchor"))
				// {
				// 	// Adding forces to connected spring entities
				// 	for (auto connectedEntity : entity.GetEntitiesByRelationshipTag("Spring"))
				// 	{
				// 		TransformComponent& connectedEntityTransform = connectedEntity.GetComponent<TransformComponent>();
				// 		RigidBodyComponent& connectedEntityRigidBody = connectedEntity.GetComponent<RigidBodyComponent>();
				//
				// 		// Adding Drag force
				// 		Vector2 drag = PhysicsEngine::GenerateDragForce(connectedEntityRigidBody, 0.008f);
				// 		connectedEntityRigidBody.AddForce(drag);
				//
				// 		// // Adding spring force
				// 		Vector2 springForce = PhysicsEngine::GenerateSpringForce(connectedEntityTransform, transform, 200, 1500);
				// 		connectedEntityRigidBody.AddForce(springForce);
				// 		rigidBody.AddForce(-springForce);
				//
				// 		// Adding weight force
				// 		Vector2 weight = Vector2(0.0f, connectedEntityRigidBody.mass * -9.8f * Physics::PIXEL_PER_METER);
				// 		connectedEntityRigidBody.AddForce(weight);
				// 	}
				// }

				// HandleEdgeCollision(entity, transform, rigidBody);

			}
		}
	}

	// Function to update Circle-Collider's center
	static void UpdateCircleColliderCenter(CircleColliderComponent& circleCollider, const TransformComponent& transform)
	{
		circleCollider.globalCenter = transform.position + circleCollider.offset;
	}

	// Function to update Box-Collider's Vertices based on transform rotation
	static void UpdateBoxColliderVertices(BoxColliderComponent& collider, const TransformComponent& transform)
	{
		// Compute global offset
		const Vector2 globalOffset = transform.position + collider.offset;

		// Precompute sine and cosine of rotation
		const float cosAngle = std::cos(transform.rotation);
		const float sinAngle = std::sin(transform.rotation);

		// Rotate and translate each vertex
		for (size_t i = 0; i < collider.localVertices.size(); ++i)
		{
			const Vector2& localVertex = collider.localVertices[i];
			collider.globalVertices[i] = Vector2(
				localVertex.x * cosAngle - localVertex.y * sinAngle,
				localVertex.x * sinAngle + localVertex.y * cosAngle
			) + globalOffset;
		}
	}

	// Function to update Polygon-Collider's Vertices based on transform rotation
	static void UpdatePolygonColliderVertices(PolygonColliderComponent& collider, const TransformComponent& transform)
	{
		// Compute global offset
		const Vector2 globalOffset = transform.position + collider.offset;

		// Precompute sine and cosine of rotation
		const float cosAngle = std::cos(transform.rotation);
		const float sinAngle = std::sin(transform.rotation);

		// Rotate and translate each vertex
		for (size_t i = 0; i < collider.localVertices.size(); ++i)
		{
			const Vector2& localVertex = collider.localVertices[i];
			collider.globalVertices[i] = Vector2(
				localVertex.x * cosAngle - localVertex.y * sinAngle,
				localVertex.x * sinAngle + localVertex.y * cosAngle
			) + globalOffset;
		}
	}

	// Only for box and circle collider. TODO: Polygon collider
	static void HandleEdgeCollision(const Entity& entity, TransformComponent& transform, RigidBodyComponent& rigidBody)
	{
		// Reversing velocity if the player collides with screen edge
		if (entity.HasComponent<BoxColliderComponent>())
		{
			const BoxColliderComponent& boxColliderComponent = entity.GetComponent<BoxColliderComponent>();

			if (transform.position.x - (boxColliderComponent.width / 2) <= 0)
			{
				transform.position.x = boxColliderComponent.width / 2;
				rigidBody.velocity.x *= -0.5f;
			}
			else if (transform.position.x + (boxColliderComponent.width / 2) >= Physics::SCREEN_WIDTH)
			{
				transform.position.x = Physics::SCREEN_WIDTH - boxColliderComponent.width / 2;
				rigidBody.velocity.x *= -0.5f;
			}

			if (transform.position.y - (boxColliderComponent.height / 2) <= 0)
			{
				transform.position.y = boxColliderComponent.height / 2;
				rigidBody.velocity.y *= -0.5f;
			}
			else if (transform.position.y + (boxColliderComponent.height / 2) >= Physics::SCREEN_HEIGHT)
			{
				transform.position.y = Physics::SCREEN_HEIGHT - boxColliderComponent.height / 2;
				rigidBody.velocity.y *= -0.5f;
			}
		}
		else if (entity.HasComponent<CircleColliderComponent>())
		{
			const CircleColliderComponent& circleColliderComponent = entity.GetComponent<CircleColliderComponent>();

			if (transform.position.x - circleColliderComponent.radius <= 0)
			{
				transform.position.x = circleColliderComponent.radius;
				rigidBody.velocity.x *= -0.5f;
			}
			else if (transform.position.x + circleColliderComponent.radius >= Physics::SCREEN_WIDTH)
			{
				transform.position.x = Physics::SCREEN_WIDTH - circleColliderComponent.radius;
				rigidBody.velocity.x *= -0.5f;
			}

			if (transform.position.y - circleColliderComponent.radius <= 0)
			{
				transform.position.y = circleColliderComponent.radius;
				rigidBody.velocity.y *= -0.5f;
			}
			else if (transform.position.y + circleColliderComponent.radius >= Physics::SCREEN_HEIGHT)
			{
				transform.position.y = Physics::SCREEN_HEIGHT - circleColliderComponent.radius;
				rigidBody.velocity.y *= -0.5f;
			}
		}
	}
};