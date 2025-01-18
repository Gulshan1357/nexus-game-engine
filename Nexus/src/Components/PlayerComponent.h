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
	// 
	int totalStrokes = 0; // Input system increments this on LaunchBall
	Ability activeAbility = Ability::NORMAL_SHOT; // Player System listens to the keyboard keys and changes the active ability
	bool bIsBallMovingAboveThresh = false; // Player System manages this by keeping track of the current velocity
	bool bIsMouseClickHold = false; // Input System check if the use is holding the attack button.

	explicit PlayerComponent(const Input::PlayerID playerId = Input::PlayerID::PLAYER_1) :
		playerId(playerId)
	{}
};