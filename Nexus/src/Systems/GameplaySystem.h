#pragma once

#include <chrono>
#include <memory>

#include "src/ECS/System.h"

struct Score;
class CollisionEvent;
class EventManager;
enum class GameState;
class LaunchBallEvent;
//------------------------------------------------------------------------
// The GameplaySystem check if ball is collided with hole
// ------------------------------------------------------------------------
class GameplaySystem : public System
{
private:
	std::chrono::steady_clock::time_point m_gameStartTime;
	const std::chrono::milliseconds m_initialDelay{ 1000 }; // 1-second delay before be start detecting for damage

	std::weak_ptr<GameState> m_gameState;
	std::weak_ptr<Score> m_score;

public:
	GameplaySystem(std::weak_ptr<GameState> gameState, std::weak_ptr<Score> m_score);

	// Subscribe to events
	void SubscribeToEvents(const std::shared_ptr<EventManager>& eventManager);

	// Callbacks
	void onBallLaunch(const LaunchBallEvent& event);
	void onCollision(const CollisionEvent& event);

	// If no player present then game over
	void Update();
	void UpdateScore(Entity& player1);
	void GameOver() const;
};