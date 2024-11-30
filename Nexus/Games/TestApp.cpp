#include "stdafx.h"
#include "TestApp.h"

#include <memory>

#include "../App/app.h"
#include "src/Components/TransformComponent.h"
#include "src/ECS/Coordinator.h"
#include "src/ECS/Entity.h"

#include "src/Utils/Logger.h"

TestApp::TestApp()
{
	Logger::Log("Game constructor called!");

	m_coordinator = std::make_unique<Coordinator>();
}
TestApp::~TestApp()
{
	Logger::Warn("Game destructor called!");
}

void TestApp::Initialize()
{
	Logger::Log("TestApp::Initialize()");

	Entity test = m_coordinator->CreateEntity();
	m_coordinator->AddComponent<TransformComponent>(test, Vector2(5.2f, 7.9f), Vector2(5.2f, 7.9f), 0.0);
	m_coordinator->KillEntity(test);
}

void TestApp::Update(float deltaTime)
{
	// Logger::Log("TestApp::Update()");
}

void TestApp::Render()
{
	App::Print(100, 100, "Sample Text");
}

void TestApp::Shutdown()
{
	Logger::Warn("TestApp::Shutdown()");
}
