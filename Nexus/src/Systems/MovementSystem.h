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

				// Adding Friction force
				Vector2 friction = PhysicsEngine::GenerateFrictionForce(entity.GetComponent<RigidBodyComponent>(), 1.0 * Physics::PIXEL_PER_METER);
				entity.GetComponent<RigidBodyComponent>().AddForce(friction);

				// Adding Gravitational Attraction force
				// Hacky way to find the other ball.
				// TODO: Don't calculate the attaction force twice, since it is the same for both object but with opposite direction.
				if (entity.HasTag("Player2"))
				{
					std::cout << "Entity A is Player 2\n";
					for (auto entity2 : GetSystemEntities())
					{

						if (entity2.HasTag("BigBall"))
						{
							std::cout << "Entity B is a BigBall\n";
							std::cout << "Entity A id: " << entity.GetId() << " Entity B id: " << entity2.GetId() << "\n";

							if (entity.GetId() != entity2.GetId())
							{
								std::cout << "Entity A != B \n";
								std::cout << "Found Gravitational force targets\n";
								Vector2 attraction = PhysicsEngine::GenerateGravitationalForce(
									entity.GetComponent<RigidBodyComponent>(),
									entity2.GetComponent<RigidBodyComponent>(),
									entity2.GetComponent<TransformComponent>().position - entity.GetComponent<TransformComponent>().position,
									1000.0,
									5,
									100);
								entity.GetComponent<RigidBodyComponent>().AddForce(attraction);
								entity2.GetComponent<RigidBodyComponent>().AddForce(-attraction);
							}
						}
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
