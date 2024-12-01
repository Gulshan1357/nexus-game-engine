#include "stdafx.h"
#include "TestApp.h"

#include <memory>

#include "../App/app.h"

#include "src/ECS/Entity.h"
#include "src/ECS/Coordinator.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/SpriteComponent.h"

#include "src/Systems/MovementSystem.h"
#include "src/Systems/RenderSystem.h"

#include "src/Utils/Vector2.h"
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

	// Add Systems
	m_coordinator->AddSystem<MovementSystem>();
	m_coordinator->AddSystem<RenderSystem>();

	// Add Entities and Components
	Entity test = m_coordinator->CreateEntity();
	test.AddComponent<TransformComponent>(Vector2(350.f, 250.f), Vector2(1.f, 1.f));
	test.AddComponent<RigidBodyComponent>(Vector2(0.02f, 0.02f));
	test.AddComponent<SpriteComponent>(); // Only prints a default sprite

	// m_coordinator->KillEntity(test);
}

// int test = 0;
void TestApp::Update(float deltaTime)
{
	// if (test <=2)
	// {
	// 	m_coordinator->CreateEntity();
	// 	m_coordinator->CreateEntity();
	// 	m_coordinator->CreateEntity();
	// 	test += 1;
	// }

	// Update the coordinator to process the entities that are waiting to be created/deleted
	m_coordinator->Update();

	// Invoke all the systems to needs to update
	m_coordinator->GetSystem<MovementSystem>().Update(deltaTime);
}

void TestApp::Render()
{
	// Update Render Systems
	m_coordinator->GetSystem<RenderSystem>().Update();
}

void TestApp::Shutdown()
{
	Logger::Warn("TestApp::Shutdown()");
}
