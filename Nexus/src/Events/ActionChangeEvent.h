#pragma once

#include "src/ECS/Entity.h"
#include "src/EventManagement/IEvent.h"
#include "src/InputManagement/InputEnums.h"

class ActionChangeEvent : public IEvent
{
public:
	Entity player;
	Input::PlayerID playerId;
	Input::PlayerAction action;
	bool isActionActive;
	ActionChangeEvent(Entity player, Input::PlayerID playerId, Input::PlayerAction action, bool isActionActive) :
		player(player), playerId(playerId), action(action), isActionActive(isActionActive)
	{}
};
