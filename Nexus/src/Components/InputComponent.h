#pragma once

#include "src/InputManagement/InputEnums.h"

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