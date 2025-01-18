#pragma once

#include "src/ECS/Entity.h"
#include "src/EventManagement/IEvent.h"

#include "Games/GalaxyGolf/AbilitiesEnum.h"

/**
 * PlayerStateChangeEvent: If a currently active ability of the player is changed then emit this event. The player system emits this event
 * @param player (Entity): Player Entity
 * @param isMoving (bool): If there is no major movement of the ball then set it to false. (To change the particle effect)
 * @param activeAbility (Ability): Active Ability
*/
class PlayerStateChangeEvent : public IEvent
{
public:
	Entity player;
	bool isMoving;
	Ability activeAbility;

	PlayerStateChangeEvent(const Entity& player, bool isMoving, const Ability& activeAbility) :
		player(player), isMoving(isMoving), activeAbility(activeAbility)
	{}
};