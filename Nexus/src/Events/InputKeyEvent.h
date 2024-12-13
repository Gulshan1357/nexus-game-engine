#pragma once

#include "src/EventManagement/IEvent.h"
#include "src/InputManagement/InputEnums.h"

class KeyPressEvent : public IEvent
{
public:
	Input::PlayerID playerId;
	Input::PlayerAction action;
	Entity player;
	KeyPressEvent(const Input::PlayerID playerId, const Input::PlayerAction action, const Entity player) :
		playerId(playerId), action(action), player(player)
	{}
};