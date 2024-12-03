#include "stdafx.h"
#include "TestApp.h"

#include <memory>

#include "../App/app.h"

#include "src/ECS/Entity.h"
#include "src/ECS/Coordinator.h"
#include <src/Event/EventBus.h>

#include "src/Components/TransformComponent.h"
#include "src/Components/SpriteComponent.h"
#include "src/Components/BoxColliderComponent.h"

#include "src/Systems/MovementSystem.h"
#include "src/Systems/RenderSystem.h"
#include "src/Systems/CollisionSystem.h"
#include "src/Systems/DamageSystem.h"

#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"

TestApp::TestApp()
{
	m_coordinator = std::make_unique<Coordinator>();
	m_eventBus = std::make_unique<EventBus>();
	Logger::Log("Game constructor called!");
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
	m_coordinator->AddSystem<CollisionSystem>();
	m_coordinator->AddSystem<DamageSystem>();

	// Add Entities and Components
	Entity test = m_coordinator->CreateEntity();
	test.AddComponent<TransformComponent>(Vector2(350.f, 250.f), Vector2(1.f, 1.f));
	test.AddComponent<RigidBodyComponent>(Vector2(0.02f, 0.0f));
	test.AddComponent<SpriteComponent>(); // Only prints a default sprite
	test.AddComponent<BoxColliderComponent>(32, 32, Vector2());
	test.Group("Player1");

	Entity test2 = m_coordinator->CreateEntity();
	test2.AddComponent<TransformComponent>(Vector2(450.f, 250.f), Vector2(1.f, 1.f));
	test2.AddComponent<RigidBodyComponent>(Vector2(-0.02f, 0.0f));
	test2.AddComponent<SpriteComponent>();
	test2.AddComponent<BoxColliderComponent>(32, 32, Vector2());
	test2.Group("Player2");

	// test2.RemoveComponent<TransformComponent>();
	// test2.AddComponent<TransformComponent>(Vector2(450.f, 250.f), Vector2(1.f, 1.f));

	// m_coordinator->KillEntity(test);
}

void TestApp::Update(float deltaTime)
{
	// Update the coordinator to process the entities that are waiting to be created/deleted
	m_coordinator->Update();

	// TODO: For event system maybe find a more performant way to just subscribing the event once instead of resetting and subscribing over and over. Maybe a buffer of subscriptions that are only added and removed at certain "events" or for a certain object ID. Example, when an entity is removed, remove all the events associated with that entity.

	// Reset all event callbacks for the current frame
	m_eventBus->Reset();

	// Perform the subscription of the events for all systems
	m_coordinator->GetSystem<DamageSystem>().SubscribeToEvents(m_eventBus);

	// Invoke all the systems that needs to be updated
	m_coordinator->GetSystem<MovementSystem>().Update(deltaTime);
	m_coordinator->GetSystem<CollisionSystem>().Update(m_eventBus);
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
