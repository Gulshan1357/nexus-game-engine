#pragma once

#include "src/ECS/Entity.h"
#include "src/EventManagement/IEvent.h"
#include "src/InputManagement/InputEnums.h"

class LaunchBallEvent : public IEvent
{
public:
	Entity player;
	Input::PlayerID playerId;
	Vector2 force;

	LaunchBallEvent(const Entity player, const Input::PlayerID playerId, const Vector2 force) :
		player(player), playerId(playerId), force(force)
	{}
};
