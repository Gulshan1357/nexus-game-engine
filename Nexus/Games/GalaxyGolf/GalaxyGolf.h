#pragma once

#include <memory>
#include <string>

#include "src/InputManagement/InputEnums.h"
#include "src/Physics/Camera.h"
#include "WorldSettings.h"

struct Score;
enum class GameState;
enum class MapType;
enum class ColliderType;
class Coordinator;
class EventManager;
class InputManager;
class AssetManager;
class AudioManager;
struct Vector2;

class GalaxyGolf
{
public:
	GalaxyGolf(WorldType worldType, std::weak_ptr<GameState> gameState, std::weak_ptr<Score> score);
	~GalaxyGolf();

	void Initialize();
	void LoadLevel(int level);
	void Update(float deltaTime);
	void ProcessInput();
	void ProcessPlayerKeys(Input::PlayerID playerId, const std::string& playerTag);
	void Render();
	void Shutdown();

private:
	//------------------------------------------------------------------------
	// Engine
	//------------------------------------------------------------------------
	bool m_isDebug; // Trigger debug mode using 'B' key

	std::unique_ptr<Coordinator> m_coordinator;
	std::shared_ptr<EventManager> m_eventManager;
	std::unique_ptr<InputManager> m_inputManager;
	std::unique_ptr<AssetManager> m_assetManager;
	std::shared_ptr<AudioManager> m_audioManager;

	//------------------------------------------------------------------------
	// World
	//------------------------------------------------------------------------
	Camera m_camera;

	// Game state and score
	WorldType m_worldType; // Level
	std::weak_ptr<GameState> m_gameState;
	std::weak_ptr<Score> m_score;

	// Level settings
	WorldSettings m_worldSettings;
	std::vector<Vector2> m_terrainVertices;
};
