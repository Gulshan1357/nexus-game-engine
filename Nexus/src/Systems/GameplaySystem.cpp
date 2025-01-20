#include "stdafx.h"
#include "GameplaySystem.h"

#include <functional>
#include <utility>

#include "PhysicsSystem.h"
#include "src/ECS/Coordinator.h"
#include "src/ECS/Entity.h"
#include "src/AssetManagement/AssetManager.h"
#include "src/EventManagement/EventManager.h"
#include "src/Events/CollisionEvent.h"
#include "src/Events/LaunchBallEvent.h"

#include "src/AudioManagement/AudioManager.h"

#include "src/Components/PlayerComponent.h"
#include "src/Components/RigidBodyComponent.h"
#include "src/Components/AnimationComponent.h"
#include "src/Components/ColliderTypeComponent.h"
#include "src/Components/CircleColliderComponent.h"
#include "src/Components/SpriteComponent.h"
#include "src/Components/TransformComponent.h"

#include "../Games/GameState.h"
#include "Games/Score.h"

#include "src/Utils/Logger.h"

GameplaySystem::GameplaySystem(std::unique_ptr<Coordinator>& coordinator, std::unique_ptr<AssetManager>& assetManager, std::shared_ptr<AudioManager> audioManager, std::weak_ptr<GameState> gameState, std::weak_ptr<Score> score)
	: m_coordinator(coordinator.get()), m_assetManager(assetManager.get()), m_audioManager(std::move(audioManager)), m_gameState(std::move(gameState)), m_score(std::move(score))
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
	Logger::Log(event.force.ToString());
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

	// Handle Explosives
	if (otherEntity.BelongsToGroup("Explosive"))
	{
		// Get the location of the entity
		// Delete this entity
		// Create a new entity, group it as ToBeDeleted, Add exploding sprite animation(non looping)
		// Create an entity refrence vector, push this entity there.
		// In update check the entities inside vector, if the animation of any reached the end if yes then delete
		const Vector2 position = otherEntity.GetComponent<TransformComponent>().position;
		otherEntity.Kill();

		m_assetManager->AddSprite("explosion", R"(.\Assets\Sprites\Obstacles\explosion4.bmp)", 1, 1);

		Entity explosionEntity = m_coordinator->CreateEntity();
		explosionEntity.AddComponent<SpriteComponent>("explosion", 3);
		explosionEntity.AddComponent<TransformComponent>(position, Vector2(0.5f, 0.5f));
		explosionEntity.Group("ToBeDeleted");
		ExplosionDetail explosionDetail = { explosionEntity, std::chrono::steady_clock::now() };
		m_explosionDetails.emplace_back(explosionDetail);

		Vector2 explosionKickBackDir = playerEntity.GetComponent<TransformComponent>().position - otherEntity.GetComponent<TransformComponent>().position;
		playerEntity.GetComponent<RigidBodyComponent>().AddForce(explosionKickBackDir * m_explosionStrength);

		if (!m_audioManager->IsAudioPlaying("explosion"))	m_audioManager->PlayAudio("explosion", false);
	}

	// if (otherEntity.BelongsToGroup("Wood"))
	// {
	// 	if (!m_audioManager->IsAudioPlaying("wood-impact")) m_audioManager->PlayAudio("wood-impact", false);
	// }
	//
	// if (otherEntity.BelongsToGroup("Stone"))
	// {
	// 	if (!m_audioManager->IsAudioPlaying("stone-impact")) m_audioManager->PlayAudio("stone-impact", false);
	// }
}

void GameplaySystem::Update(std::vector<Vector2>& terrainVertices)
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
		auto& playerPosition = player.GetComponent<TransformComponent>().position;
		auto& playerVelocity = player.GetComponent<RigidBodyComponent>().velocity;
		float minX = terrainVertices.front().x;
		float maxX = terrainVertices.back().x;
		float minY =  -1000.f;
		float maxY = 3000.f;
		// Check X bounds
		if (playerPosition.x < minX)
		{
			playerPosition.x = minX;
			playerVelocity.x *= -1.0f;  // Reverse x velocity
		}
		else if (playerPosition.x > maxX)
		{
			playerPosition.x = maxX;
			playerVelocity.x *= -1.0f;  // Reverse x velocity
		}

		// Check Y bounds
		if (playerPosition.y < minY)
		{
			playerPosition.y = minY;
			playerVelocity.y *= -1.0f;  // Reverse y velocity
		}
		else if (playerPosition.y > maxY)
		{
			playerPosition.y = maxY;
			playerVelocity.y *= -1.0f;  // Reverse y velocity
		}

	}
	if (GetSystemEntities().empty())
	{
		GameOver();
	}

	// Remove the explosion sprites
	for (auto& [entity, explodedAt] : m_explosionDetails)
	{
		const auto timeSinceExplosion = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - explodedAt);

		if (timeSinceExplosion > m_explosionTimeout)
		{
			m_coordinator->KillEntity(entity);
		}
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

