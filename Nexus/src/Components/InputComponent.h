#pragma once

#include "src/InputManagement/InputEnums.h"

struct InputComponent
{
	Input::PlayerID playerId;
	float upVelocity;
	float rightVelocity;
	float downVelocity;
	float leftVelocity;

	InputComponent(Input::PlayerID playerId = Input::PlayerID::PLAYER_1, float upVelocity = 0.0f, float rightVelocity = 0.0f, float downVelocity = 0.0f, float leftVelocity = float()) :
		playerId(playerId),
		upVelocity(upVelocity),
		rightVelocity(rightVelocity),
		downVelocity(downVelocity),
		leftVelocity(leftVelocity)
	{}
};