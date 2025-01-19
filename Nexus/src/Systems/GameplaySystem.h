#pragma once

#include <chrono>
#include <memory>

#include "src/ECS/System.h"

class AudioManager;
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
	std::chrono::milliseconds m_initialDelay{ 1000 }; // 1-second delay before we start detecting for collisions

	std::shared_ptr<AudioManager> m_audioManager;
	std::weak_ptr<GameState> m_gameState;
	std::weak_ptr<Score> m_score;

	// Delay to make sure the ball is resting inside the hole
	bool m_ballInHole = false;
	std::chrono::steady_clock::time_point m_holeEnterTime;
	std::chrono::steady_clock::time_point m_lastHoleCollisionTime;
	std::chrono::milliseconds m_holeDelay{ 1000 }; // Ball should stay in hole for 1 sec
	std::chrono::milliseconds m_collisionTimeout{ 500 }; // 0.5 second timeout. In case ball leaves a hole after just touching, restart.

public:
	GameplaySystem(std::shared_ptr<AudioManager> audioManager, std::weak_ptr<GameState> gameState, std::weak_ptr<Score> m_score);

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