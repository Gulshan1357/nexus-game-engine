#pragma once

#include <iostream>

#include "src/ECS/Entity.h"
#include "src/ECS/System.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/RigidBodyComponent.h"

#include "src/Physics/PhysicsEngine.h"
#include "src/Physics/Constants.h"

class MovementSystem : public System
{
public:
	MovementSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void Update(float deltaTime) const
	{
		for (auto entity : GetSystemEntities())
		{
			TransformComponent& transform = entity.GetComponent<TransformComponent>();
			RigidBodyComponent& rigidBody = entity.GetComponent<RigidBodyComponent>();

			// Delta time is in milliseconds so dividing by 1000 to convert it into seconds
			float dt = deltaTime / 1000;

			if (!rigidBody.bUsePhysics)
			{
				rigidBody.velocity += rigidBody.acceleration * dt;
				transform.position += rigidBody.velocity * dt;
			}
			else
			{
				// Integrating all the forces acting on the rigidbody using Physics Engine
				transform.position += PhysicsEngine::Integrate(rigidBody, dt);

				// Tracking the location of anchor (Player2) so that we can attach Big ball with spring force
				if (entity.HasTag("Player2"))
				{
					// Adding forces to connected spring entities
					for (auto connectedEntity : entity.GetEntitiesByRelationshipTag("Spring"))
					{
						TransformComponent& connectedEntityTransform = connectedEntity.GetComponent<TransformComponent>();
						RigidBodyComponent& connectedEntityRigidBody = connectedEntity.GetComponent<RigidBodyComponent>();

						// Adding Drag force
						Vector2 drag = PhysicsEngine::GenerateDragForce(connectedEntityRigidBody, 0.03f);
						connectedEntityRigidBody.AddForce(drag);

						// Adding weight force
						Vector2 weight = Vector2(0.0f, connectedEntityRigidBody.mass * -9.8f * Physics::PIXEL_PER_METER);
						connectedEntityRigidBody.AddForce(weight);

						// Adding spring force
						Vector2 springForce = PhysicsEngine::GenerateSpringForce(connectedEntityTransform, transform, 200, 100);
						connectedEntityRigidBody.AddForce(springForce);
					}
				}

				// Reversing velocity if the player collides with screen edge
				if (entity.HasComponent<BoxColliderComponent>())
				{
					const BoxColliderComponent& boxColliderComponent = entity.GetComponent<BoxColliderComponent>();

					if (transform.position.x - (boxColliderComponent.width / 2) <= 0)
					{
						transform.position.x = boxColliderComponent.width / 2;
						rigidBody.velocity.x *= -0.9f;
					}
					else if (transform.position.x + (boxColliderComponent.width / 2) >= Physics::SCREEN_WIDTH)
					{
						transform.position.x = Physics::SCREEN_WIDTH - boxColliderComponent.width / 2;
						rigidBody.velocity.x *= -0.9f;
					}

					if (transform.position.y - (boxColliderComponent.height / 2) <= 0)
					{
						transform.position.y = boxColliderComponent.height / 2;
						rigidBody.velocity.y *= -0.9f;
					}
					else if (transform.position.y + (boxColliderComponent.height / 2) >= Physics::SCREEN_HEIGHT)
					{
						transform.position.y = Physics::SCREEN_HEIGHT - boxColliderComponent.height / 2;
						rigidBody.velocity.y *= -0.9f;
					}
				}
			}
		}
	}
};
