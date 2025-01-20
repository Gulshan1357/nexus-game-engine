#include "stdafx.h"
#include "GameplaySystem.h"

#include <functional>
#include <utility>

#include "src/ECS/Coordinator.h"
#include "src/ECS/Entity.h"

#include "src/EventManagement/EventManager.h"
#include "src/Events/CollisionEvent.h"
#include "src/Events/LaunchBallEvent.h"

#include "src/AudioManagement/AudioManager.h"

#include "src/Components/PlayerComponent.h"
#include "src/Components/RigidBodyComponent.h"

#include "../Games/GameState.h"
#include "Games/Score.h"
#include "src/Utils/Logger.h"

GameplaySystem::GameplaySystem(std::shared_ptr<AudioManager> audioManager, std::weak_ptr<GameState> gameState, std::weak_ptr<Score> score)
	: m_audioManager(std::move(audioManager)), m_gameState(std::move(gameState)), m_score(std::move(score))
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
	m_audioManager->PlayAudio("golf_swing");
}

void GameplaySystem::onCollision(const CollisionEvent& event)
{
	// Ignore collision for 1 sec
	const auto currentTime = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_gameStartTime) < m_initialDelay)
		return;

	// Check if either entity is a player
	const bool isAPlayer = event.a.BelongsToGroup("Player");
	const bool isBPlayer = event.b.BelongsToGroup("Player");

	// If neither is a player, exit early
	if (!isAPlayer && !isBPlayer)
		return;

	const auto& playerEntity = isAPlayer ? event.a : event.b;
	const auto& otherEntity = isAPlayer ? event.b : event.a;

	// Handle Killers
	if (otherEntity.BelongsToGroup("StaticKillers"))
	{
		playerEntity.Kill();
		return;
	}

	// Handle Hole
	if (otherEntity.HasTag("Hole"))
	{
		m_lastHoleCollisionTime = currentTime;

		if (!m_ballInHole)
		{
			m_ballInHole = true;
			m_holeEnterTime = currentTime;
		}

		const auto timeInHole = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_holeEnterTime);
		if (timeInHole >= m_holeDelay)
		{
			Logger::Log("Hole!");
			playerEntity.Kill();
			m_ballInHole = false;
		}
	}
}

void GameplaySystem::Update()
{
	const auto currentTime = std::chrono::steady_clock::now();
	// Check if we haven't received a hole collision recently
	const auto timeSinceLastCollision = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastHoleCollisionTime);
	if (timeSinceLastCollision > m_collisionTimeout)
	{
		// No collision between ball and hole for more than 0.5 seconds, reset state
		m_ballInHole = false;
	}

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

