#pragma once

#include <memory>
#include <string>

#include "src/InputManagement/InputEnums.h"

class Coordinator;
class EventBus;
class KeyBindings;

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
	std::unique_ptr<EventBus> m_eventBus;
	std::unique_ptr<KeyBindings> m_keyBindings;
};
