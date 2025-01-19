#include "stdafx.h"
#include "GameplaySystem.h"

#include <functional>

#include "src/ECS/Coordinator.h"
#include "src/ECS/Entity.h"

#include "src/EventManagement/EventManager.h"
#include "src/Events/CollisionEvent.h"
#include "src/Events/LaunchBallEvent.h"

#include "src/Components/PlayerComponent.h"
#include "src/Components/RigidBodyComponent.h"

#include "../Games/GameState.h"
#include "Games/Score.h"
#include "src/Utils/Logger.h"

GameplaySystem::GameplaySystem(std::weak_ptr<GameState> gameState, std::weak_ptr<Score> score)
	: m_gameState(std::move(gameState)), m_score(std::move(score))
{
	RequireComponent<PlayerComponent>();
	m_gameStartTime = std::chrono::steady_clock::now();
}

void GameplaySystem::SubscribeToEvents(const std::shared_ptr<EventManager>& eventManager)
{
	// Collision Event
	const std::function<void(GameplaySystem*, CollisionEvent&)> callbackCollision = [this](auto&&, auto&& placeholder2) { onCollision(std::forward<decltype(placeholder2)>(placeholder2)); };
	eventManager->SubscribeToEvent<CollisionEvent>(this, callbackCollision);
	//  std::placeholders::_2 tells std::bind that the second argument (the event) will be provided when the resulting function is called. This allows us to create a callable object that matches the required function signature of SubscribeToEvent, where the first argument is the instance (DamageSystem*), and the second argument is the event (CollisionEvent&).

	// Launch Ball Event
	const std::function<void(GameplaySystem*, LaunchBallEvent&)> callbackLaunch = [this](auto&&, auto&& placeholder2) { onBallLaunch(std::forward<decltype(placeholder2)>(placeholder2)); };
	eventManager->SubscribeToEvent<LaunchBallEvent>(this, callbackLaunch);

}

void GameplaySystem::onBallLaunch(const LaunchBallEvent& event)
{
	event.player.GetComponent<RigidBodyComponent>().AddForce(event.force);
	auto& playerComponent = event.player.GetComponent<PlayerComponent>();

	playerComponent.totalStrokes += 1;
	Logger::Log("onBallLaunch() from Gameplaye system");
}

void GameplaySystem::onCollision(const CollisionEvent& event)
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

void GameplaySystem::Update()
{
	for (auto& player : GetSystemEntities())
	{
		UpdateScore(player);
	}
	if (GetSystemEntities().empty())
	{
		GameOver();
	}
}

void GameplaySystem::UpdateScore(Entity& player1)
{
	if (auto sc = m_score.lock())
	{
		// Update Score based on the player component so that the GameOverScreen can display it
		try
		{
			sc->playerOneTotalShots = player1.GetComponent<PlayerComponent>().totalStrokes;
		}
		catch ([[maybe_unused]] const std::out_of_range& e)
		{
			Logger::Err("GameplaySystem::UpdateScore(): Couldn't find the entity with player tag");
		}
	}
	else
	{
		Logger::Err("GameplaySystem::UpdateScore(): Score is no longer valid.");
	}
}

void GameplaySystem::GameOver() const
{
	if (auto gmState = m_gameState.lock())
	{
		*gmState = GameState::GAME_OVER;
	}
	else
	{
		Logger::Err("GameplaySystem::GameOver() GameState is no longer valid.");
	}
}

