#pragma once

#include <memory>
#include <functional>

#include "src/ECS/System.h"

#include "src/EventManagement/EventManager.h"
#include "src/Events/CollisionEvent.h"

#include "src/Utils/Logger.h"

//------------------------------------------------------------------------
// The GameplaySystem check if ball is collided with hole
// ------------------------------------------------------------------------
class GameplaySystem : public System
{
public:
	GameplaySystem()
	{
		// RequireComponent<PlayerComponent>();
	}

	void SubscribeToEvents(const std::unique_ptr<EventManager>& eventManager)
	{
		using CallbackType = std::function<void(GameplaySystem*, CollisionEvent&)>;
		const CallbackType callback = [this](auto&&, auto&& placeholder2) { onCollision(std::forward<decltype(placeholder2)>(placeholder2)); };
		eventManager->SubscribeToEvent<CollisionEvent>(this, callback);
		//  std::placeholders::_2 tells std::bind that the second argument (the event) will be provided when the resulting function is called. This allows us to create a callable object that matches the required function signature of SubscribeToEvent, where the first argument is the instance (DamageSystem*), and the second argument is the event (CollisionEvent&).
	}

	void onCollision(const CollisionEvent& event)
	{
		// Logger::Log("Gameplay system received an event collision between entities " + std::to_string(event.a.GetId()) + " and " + std::to_string(event.b.GetId()));

		if ((event.a.BelongsToGroup("Player") && event.b.HasTag("Hole")) ||
			(event.b.BelongsToGroup("Player") && event.a.HasTag("Hole")))
		{
			Logger::Log("Hole!");
		}
	}
};