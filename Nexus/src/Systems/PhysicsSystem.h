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
		Logger::Log("Initializing Entity Physics. Calculating inverse mass, angular mass etc.");
		// Calculate inverse mass, angular mass and inverse angular mass of entities with RigidBody
		for (auto entity : GetSystemEntities())
		{
			auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
			if (rigidBody.bUsePhysics)
			{

				rigidBody.inverseOfMass = (rigidBody.mass != 0.0f) ? 1.0f / rigidBody.mass : 0.0f;

				if (entity.HasComponent<ColliderTypeComponent>())
				{
					rigidBody.angularMass = PhysicsEngine::CalculateMomentOfInertia(entity);
					rigidBody.inverseOfAngularMass = (rigidBody.angularMass != 0.0f) ? 1.0f / rigidBody.angularMass : 0.0f;
					if (entity.HasTag("Debug"))
					{
						std::cout << "Debug entity, mass: " << rigidBody.mass << " I: " << rigidBody.angularMass << " inverse of I: " << rigidBody.inverseOfAngularMass << "\n";
					}
				}
			}
		}
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

				if (entity.HasComponent<BoxColliderComponent>())
				{
					UpdateColliderVertices(entity.GetComponent<BoxColliderComponent>(), transform);
				}
				else if (entity.HasComponent<PolygonColliderComponent>())
				{
					UpdateColliderVertices(entity.GetComponent<PolygonColliderComponent>(), transform);
				}

				HandleEdgeCollision(entity, transform, rigidBody);

				if (entity.HasTag("Player2") || entity.HasTag("BigBall") || entity.BelongsToGroup("Anchor"))
				{
					// Adding forces to connected spring entities
					for (auto connectedEntity : entity.GetEntitiesByRelationshipTag("Spring"))
					{
						TransformComponent& connectedEntityTransform = connectedEntity.GetComponent<TransformComponent>();
						RigidBodyComponent& connectedEntityRigidBody = connectedEntity.GetComponent<RigidBodyComponent>();

						// Adding Drag force
						Vector2 drag = PhysicsEngine::GenerateDragForce(connectedEntityRigidBody, 0.008f);
						connectedEntityRigidBody.AddForce(drag);

						// Adding spring force
						Vector2 springForce = PhysicsEngine::GenerateSpringForce(connectedEntityTransform, transform, 200, 1500);
						connectedEntityRigidBody.AddForce(springForce);
						rigidBody.AddForce(-springForce);

						// Adding weight force
						Vector2 weight = Vector2(0.0f, connectedEntityRigidBody.mass * -9.8f * Physics::PIXEL_PER_METER);
						connectedEntityRigidBody.AddForce(weight);
					}
				}

			}
		}
	}

	// Function to update Box Collider Vertices based on transform rotation
	static void UpdateColliderVertices(BoxColliderComponent& collider, const TransformComponent& transform)
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

	// Function to update Polygon Collider Vertices based on transform rotation
	static void UpdateColliderVertices(PolygonColliderComponent& collider, const TransformComponent& transform)
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