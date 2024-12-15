#pragma once

#include "src/InputManagement/InputEnums.h"

/**
 * InputComponent Component provides information for Input System
 * @param playerId Player ID (probably not needed, TODO: check if can be removed)
 * @param upVelocity velocity on Move_Up input action
 * @param rightVelocity velocity on Move_Right input action
 * @param downVelocity velocity on Move_Down input action
 * @param leftVelocity velocity on Move_Left input action
*/
struct InputComponent
{
	Input::PlayerID playerId;
	float upVelocity;
	float rightVelocity;
	float downVelocity;
	float leftVelocity;

	explicit InputComponent(const Input::PlayerID playerId = Input::PlayerID::PLAYER_1, const float upVelocity = 0.0f, const float rightVelocity = 0.0f, const float downVelocity = 0.0f, const float leftVelocity = 0.0f) :
		playerId(playerId),
		upVelocity(upVelocity),
		rightVelocity(rightVelocity),
		downVelocity(downVelocity),
		leftVelocity(leftVelocity)
	{}
};