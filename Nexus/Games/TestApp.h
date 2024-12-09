#pragma once

#include <memory>
#include <string>

#include "src/InputManagement/InputEnums.h"

class Coordinator;
class EventManager;
class KeyBindings;
class AssetManager;

class TestApp
{
public:
	TestApp();
	~TestApp();

	void Initialize();
	void LoadLevel(int level);
	void Update(float deltaTime);
	void ProcessInput();
	void ProcessPlayerKeys(Input::PlayerID playerId, const std::string& playerTag);
	void Render();
	void Shutdown();

private:
	std::unique_ptr<Coordinator> m_coordinator;
	std::unique_ptr<EventManager> m_eventManager;
	std::unique_ptr<KeyBindings> m_keyBindings;
	std::unique_ptr<AssetManager> m_assetManager;
};
