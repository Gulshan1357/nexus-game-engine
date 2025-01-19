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

	void Update(const std::shared_ptr<EventManager>& eventManager) const
	{
		static constexpr float velocity_threshold = 5000.0f;

		for (auto& entity : GetSystemEntities())
		{
			auto& player = entity.GetComponent<PlayerComponent>();
			const auto& rigidbody = entity.GetComponent<RigidBodyComponent>();

			const float currentVelocity = rigidbody.velocity.MagnitudeSquared();

			// Check player speed to emit event from/to idle position
			if (player.bIsBallMovingAboveThresh)
			{
				// Check if player has stopped (velocity dropped below threshold)
				if (currentVelocity < velocity_threshold)
				{
					player.bIsBallMovingAboveThresh = false;
					eventManager->EmitEvent<PlayerStateChangeEvent>(entity, player.bIsBallMovingAboveThresh, player.activeAbility);
				}
			}
			else
			{
				// Check if player has started moving (velocity exceeded threshold)
				if (currentVelocity > velocity_threshold)
				{
					player.bIsBallMovingAboveThresh = true;
					eventManager->EmitEvent<PlayerStateChangeEvent>(entity, player.bIsBallMovingAboveThresh, player.activeAbility);
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
		}
	}

};
