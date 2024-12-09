#include "stdafx.h"
#include "TestApp.h"

#include <memory>

#include "../App/app.h"

#include "src/ECS/Entity.h"
#include "src/ECS/Coordinator.h"
#include "src/EventManagement/EventManager.h"
#include "src/InputManagement/KeyBindings.h"
#include "src/InputManagement/InputEnums.h"
#include "src/AssetManagement/AssetManager.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/SpriteComponent.h"
#include "src/Components/BoxColliderComponent.h"
#include "src/Components/UITextComponent.h"
#include "src/Components/InputComponent.h"

#include "src/Systems/MovementSystem.h"
#include "src/Systems/RenderSystem.h"
#include "src/Systems/CollisionSystem.h"
#include "src/Systems/DamageSystem.h"
#include "src/Systems/RenderTextSystem.h"
#include "src/Systems/InputSystem.h"

#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"

#include "src/Events/KeyPressEvent.h"

TestApp::TestApp()
{
	m_coordinator = std::make_unique<Coordinator>();
	m_eventManager = std::make_unique<EventManager>();
	m_keyBindings = std::make_unique<KeyBindings>();
	m_assetManager = std::make_unique<AssetManager>();
	Logger::Log("Game constructor called!");
}
TestApp::~TestApp()
{
	Logger::Warn("Game destructor called!");
}

void TestApp::Initialize()
{
	Logger::Log("TestApp::Initialize()");

	LoadLevel(1);
}

void TestApp::LoadLevel(int level)
{
	// Add Systems
	m_coordinator->AddSystem<MovementSystem>();
	m_coordinator->AddSystem<RenderSystem>();
	m_coordinator->AddSystem<CollisionSystem>();
	m_coordinator->AddSystem<DamageSystem>();
	m_coordinator->AddSystem<RenderTextSystem>();
	m_coordinator->AddSystem<InputSystem>();

	// Add assets to the asset manager
	m_assetManager->AddSprite("test-image", R"(.\Assets\Sprites\Test.bmp)", 8, 4);

	// Add Entities and Components
	Entity test = m_coordinator->CreateEntity();
	test.AddComponent<TransformComponent>(Vector2(350.f, 250.f), Vector2(1.f, 1.f));
	test.AddComponent<RigidBodyComponent>(Vector2(0.00f, 0.0f));
	test.AddComponent<SpriteComponent>("test-image"); // Only prints a default sprite
	test.AddComponent<BoxColliderComponent>(32, 32, Vector2());
	test.Tag("Player1");
	test.AddComponent<InputComponent>(Input::PlayerID::PLAYER_1, Vector2(0, 0.018f), Vector2(0.018f, 0), Vector2(0, -0.018f), Vector2(-0.018f, 0));

	Entity test2 = m_coordinator->CreateEntity();
	test2.AddComponent<TransformComponent>(Vector2(450.f, 250.f), Vector2(1.f, 1.f));
	test2.AddComponent<RigidBodyComponent>(Vector2(-0.00f, 0.0f));
	test2.AddComponent<SpriteComponent>("test-image");
	test2.AddComponent<BoxColliderComponent>(32, 32, Vector2());
	test2.AddComponent<InputComponent>(Input::PlayerID::PLAYER_2, Vector2(0, 0.018f), Vector2(0.018f, 0), Vector2(0, -0.018f), Vector2(-0.018f, 0));
	test2.Tag("Player2");

	Entity uiTextHello = m_coordinator->CreateEntity();
	uiTextHello.AddComponent<UITextComponent>("New Render Text System!", Vector2(100, 100), Color(Colors::CYAN), FontType::HELVETICA_18);

	// test2.RemoveComponent<TransformComponent>();
	// test2.AddComponent<TransformComponent>(Vector2(450.f, 250.f), Vector2(1.f, 1.f));

	// m_coordinator->KillEntity(test);

	// Add Key bindings for players
	// Player 1
	m_keyBindings->AddKeyBinding(Input::PlayerID::PLAYER_1, 'W', Input::PlayerAction::MOVE_UP);
	m_keyBindings->AddKeyBinding(Input::PlayerID::PLAYER_1, 'D', Input::PlayerAction::MOVE_RIGHT);
	m_keyBindings->AddKeyBinding(Input::PlayerID::PLAYER_1, 'S', Input::PlayerAction::MOVE_DOWN);
	m_keyBindings->AddKeyBinding(Input::PlayerID::PLAYER_1, 'A', Input::PlayerAction::MOVE_LEFT);

	// Player 2
	m_keyBindings->AddKeyBinding(Input::PlayerID::PLAYER_2, VK_UP, Input::PlayerAction::MOVE_UP);
	m_keyBindings->AddKeyBinding(Input::PlayerID::PLAYER_2, VK_RIGHT, Input::PlayerAction::MOVE_RIGHT);
	m_keyBindings->AddKeyBinding(Input::PlayerID::PLAYER_2, VK_DOWN, Input::PlayerAction::MOVE_DOWN);
	m_keyBindings->AddKeyBinding(Input::PlayerID::PLAYER_2, VK_LEFT, Input::PlayerAction::MOVE_LEFT);
}

void TestApp::Update(float deltaTime)
{
	// TODO: For event system maybe find a more performant way to just subscribing the event once instead of resetting and subscribing over and over. Maybe a buffer of subscriptions that are only added and removed at certain "events" or for a certain object ID. Example, when an entity is removed, remove all the events associated with that entity.

	// Reset all event callbacks for the current frame
	m_eventManager->Reset();

	// Perform the subscription of the events for all systems
	m_coordinator->GetSystem<DamageSystem>().SubscribeToEvents(m_eventManager);
	m_coordinator->GetSystem<InputSystem>().SubscribeToEvents(m_eventManager);

	// Update the coordinator to process the entities that are waiting to be created/deleted
	m_coordinator->Update();

	// Invoke all the systems that needs to be updated
	m_coordinator->GetSystem<MovementSystem>().Update(deltaTime);
	m_coordinator->GetSystem<CollisionSystem>().Update(m_eventManager);

	ProcessInput();
}

//------------------------------------------------------------------------
// Process key input for each player
//------------------------------------------------------------------------
void TestApp::ProcessInput()
{
	ProcessPlayerKeys(Input::PlayerID::PLAYER_1, "Player1");
	ProcessPlayerKeys(Input::PlayerID::PLAYER_2, "Player2");
}

//------------------------------------------------------------------------
// Loop all player keys and check is any is currently clicked
//------------------------------------------------------------------------
void TestApp::ProcessPlayerKeys(Input::PlayerID playerId, const std::string& playerTag)
{
	for (const auto key : m_keyBindings->GetAllKeys(playerId))
	{
		if (App::IsKeyPressed(key))
		{
			// Send the Keypress event by mapping key to action using KeyBindings::GetAction();
			m_eventManager->EmitEvent<KeyPressEvent>(
				playerId,
				m_keyBindings->GetAction(playerId, key),
				m_coordinator->GetEntityByTag(playerTag)
			);
		}
	}
}

void TestApp::Render()
{
	// Update Render Systems
	m_coordinator->GetSystem<RenderSystem>().Update(m_assetManager);
	m_coordinator->GetSystem<RenderTextSystem>().Update();
}

void TestApp::Shutdown()
{
	Logger::Warn("TestApp::Shutdown()");
}
