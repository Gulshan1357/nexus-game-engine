#pragma once

#include <memory>
#include <string>

#include "src/InputManagement/InputEnums.h"
#include "src/Physics/Constants.h"

enum class ColliderType;
class Coordinator;
class EventManager;
class InputManager;
class AssetManager;
class AudioManager;
struct Vector2;

class TestApp
{
public:
	TestApp();
	~TestApp();

	void Initialize();
	void LoadLevel(int level);
	void PrintTiles(const std::string& tileMapAssetId, float scale, const std::string& mapFileLocation, int rows, int cols);
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
	bool m_bWasLMousePressedPast = false;

	std::unique_ptr<Coordinator> m_coordinator;
	std::unique_ptr<EventManager> m_eventManager;
	std::unique_ptr<InputManager> m_inputManager;
	std::unique_ptr<AssetManager> m_assetManager;
	std::unique_ptr<AudioManager> m_audioManager;

	//------------------------------------------------------------------------
	// World
	//------------------------------------------------------------------------

	float G = -Physics::gravity;
	void SpawnShape(Vector2 position, ColliderType colliderType) const;
};
