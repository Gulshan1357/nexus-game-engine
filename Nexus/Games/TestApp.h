#pragma once

#include <memory>

class Coordinator;
class EventBus;

class TestApp
{
public:
	TestApp();
	~TestApp();

	void Initialize();
	void Update(float deltaTime);
	void Render();
	void Shutdown();

private:
	std::unique_ptr<Coordinator> m_coordinator;
	std::unique_ptr<EventBus> m_eventBus;
};
