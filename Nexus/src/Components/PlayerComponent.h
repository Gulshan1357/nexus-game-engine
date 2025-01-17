#pragma once

#include "src/InputManagement/InputEnums.h"

/**
 * PlayerComponent Component provides information for Input System and Player System
 * @param playerId Player ID (probably not needed, TODO: check if can be removed)
*/
struct PlayerComponent
{
	Input::PlayerID playerId;

	explicit PlayerComponent(const Input::PlayerID playerId = Input::PlayerID::PLAYER_1) :
		playerId(playerId)
	{}
};