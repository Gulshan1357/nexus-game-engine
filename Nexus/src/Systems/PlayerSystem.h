#pragma once

#include <memory>

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/PlayerComponent.h"
#include "src/Components/RigidBodyComponent.h"
#include "src/EventManagement/EventManager.h"
#include "src/Events/PlayerStateChangeEvent.h"

class PlayerSystem : public System
{
public:
	PlayerSystem()
	{
		RequireComponent<PlayerComponent>();
	}

	void Update(const std::unique_ptr<EventManager>& eventManager) const
	{
		static constexpr float velocity_threshold = 5000.0f;

		for (auto& entity : GetSystemEntities())
		{
			auto& player = entity.GetComponent<PlayerComponent>();
			const auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

			const float currentVelocity = rigidbody.velocity.MagnitudeSquared();

			// Check player speed to emit event from/to idle position
			if (player.bIsMoving)
			{
				// Check if player has stopped (velocity dropped below threshold)
				if (currentVelocity < velocity_threshold)
				{
					player.bIsMoving = false;
					eventManager->EmitEvent<PlayerStateChangeEvent>(entity, player.bIsMoving, player.activeAbility);
				}
			}
			else
			{
				// Check if player has started moving (velocity exceeded threshold)
				if (currentVelocity > velocity_threshold)
				{
					player.bIsMoving = true;
					eventManager->EmitEvent<PlayerStateChangeEvent>(entity, player.bIsMoving, player.activeAbility);
				}
			}

			// Use 1, 2, 3 to trigger normal, power and sniper shots
			if (App::IsKeyPressed('1'))
			{
				player.activeAbility = Ability::NORMAL_SHOT;
			}
			if (App::IsKeyPressed('2'))
			{
				player.activeAbility = Ability::POWER_SHOT;
			}
			if (App::IsKeyPressed('3'))
			{
				player.activeAbility = Ability::SNIPER_SHOT;
			}

			Logger::Log(std::to_string(rigidbody.velocity.MagnitudeSquared()));
		}
	}

};
