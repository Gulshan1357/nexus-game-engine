#pragma once

#include "Games/GalaxyGolf/AbilitiesEnum.h"
#include "src/InputManagement/InputEnums.h"

/**
 * PlayerComponent Component provides information for Input System and Player System
 * @param playerId Player ID (probably not needed, TODO: check if can be removed)
*/
struct PlayerComponent
{
	Input::PlayerID playerId;

	// Managed by other system
	int totalStrokes = 0; // Input system increments this on LaunchBall
	bool bIsMoving = false; // If change the value when ball is moving for particle effect
	Ability activeAbility = Ability::NORMAL_SHOT;

	explicit PlayerComponent(const Input::PlayerID playerId = Input::PlayerID::PLAYER_1) :
		playerId(playerId)
	{}
};