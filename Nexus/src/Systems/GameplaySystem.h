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
private:
	std::chrono::steady_clock::time_point m_gameStartTime;
	const std::chrono::milliseconds m_initialDelay{ 1000 }; // 1-second delay before be start detecting for damage

public:
	GameplaySystem()
	{
		m_gameStartTime = std::chrono::steady_clock::now();
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
		// Ignore collision for 1 sec
		const auto currentTime = std::chrono::steady_clock::now();
		const auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_gameStartTime);
		if (timeElapsed < m_initialDelay) return;

		// Groups : Killers (Kill player)
		if (event.a.BelongsToGroup("Killers") && event.b.BelongsToGroup("Player"))
		{
			event.b.Kill();
		}
		else if (event.b.BelongsToGroup("Killers") && event.a.BelongsToGroup("Player"))
		{
			event.a.Kill();
		}

		// Hole
		if ((event.a.BelongsToGroup("Player") && event.b.HasTag("Hole")) ||
			(event.b.BelongsToGroup("Player") && event.a.HasTag("Hole")))
		{
			Logger::Log("Hole!");
		}
	}
};