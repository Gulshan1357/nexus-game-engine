#pragma once

#include "src/Utils/Vector2.h"
#include "src/InputManagement/InputEnums.h"

struct InputComponent
{
	Input::PlayerID playerId;
	Vector2 upVelocity;
	Vector2 rightVelocity;
	Vector2 downVelocity;
	Vector2 leftVelocity;

	InputComponent(Input::PlayerID playerId = Input::PlayerID::PLAYER_1, Vector2 upVelocity = Vector2(), Vector2 rightVelocity = Vector2(), Vector2 downVelocity = Vector2(), Vector2 leftVelocity = Vector2()) :
		playerId(playerId),
		upVelocity(upVelocity),
		rightVelocity(rightVelocity),
		downVelocity(downVelocity),
		leftVelocity(leftVelocity)
	{}
};