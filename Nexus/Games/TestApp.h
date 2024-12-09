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
	void ProcessInput();
	void ProcessPlayerKeys(Input::PlayerID playerId, const std::string& playerTag);
	void Update(float deltaTime);
	void Render();
	void Shutdown();

private:
	std::unique_ptr<Coordinator> m_coordinator;
	std::unique_ptr<EventManager> m_eventManager;
	std::unique_ptr<KeyBindings> m_keyBindings;
	std::unique_ptr<AssetManager> m_assetManager;
};
