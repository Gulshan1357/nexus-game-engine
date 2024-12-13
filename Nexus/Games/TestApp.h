#pragma once

#include <memory>
#include <string>

#include "src/InputManagement/InputEnums.h"

class Coordinator;
class EventManager;
class InputManager;
class AssetManager;

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
	bool bIsDebug; // Trigger debug mode using 'B' key

	std::unique_ptr<Coordinator> m_coordinator;
	std::unique_ptr<EventManager> m_eventManager;
	std::unique_ptr<InputManager> m_inputManager;
	std::unique_ptr<AssetManager> m_assetManager;
};
