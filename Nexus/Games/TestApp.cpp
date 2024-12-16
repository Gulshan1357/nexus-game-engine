#include "stdafx.h"
#include "TestApp.h"

#include <memory>
#include <fstream>

#include "../App/app.h"

#include "src/ECS/Entity.h"
#include "src/ECS/Coordinator.h"
#include "src/EventManagement/EventManager.h"
#include "src/InputManagement/InputManager.h"
#include "src/InputManagement/InputEnums.h"
#include "src/AssetManagement/AssetManager.h"
#include "src/AssetManagement/AssetEnums.h"

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
#include "src/Systems/RenderDebugSystem.h"

#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"

#include "src/Components/AnimationComponent.h"
#include <src/Systems/AnimationSystem.h>

#include "src/Events/ActionChangeEvent.h"

TestApp::TestApp()
{
	bIsDebug = false;

	m_coordinator = std::make_unique<Coordinator>();
	m_eventManager = std::make_unique<EventManager>();
	m_inputManager = std::make_unique<InputManager>();
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
	m_coordinator->AddSystem<RenderDebugSystem>();
	m_coordinator->AddSystem<AnimationSystem>();

	// Add assets to the asset manager
	m_assetManager->AddSprite("player1-test-image", R"(.\Assets\Sprites\Test.bmp)", 8, 4);
	m_assetManager->AddSprite("player2-test-image", R"(.\Assets\Sprites\Test.bmp)", 8, 4);
	m_assetManager->AddSprite("tile-map", R"(.\Assets\Sprites\tilesheet.bmp)", 21, 8);
	m_assetManager->AddSprite("blue-ball", R"(.\Assets\Sprites\ball_blue_small.bmp)", 1, 1);
	m_assetManager->AddSprite("red-ball", R"(.\Assets\Sprites\ball_red_small.bmp)", 1, 1);

	// Print TileMaps
	PrintTiles("tile-map", static_cast<float>(0.8), R"(.\Assets\Sprites\test2.map)", 20, 20);

	// Animation SystemLimitation: If one player is animating then all should animation component otherwise different player sprite start animating on different player's input
	// Add Entities and Components
	Entity test = m_coordinator->CreateEntity();
	test.AddComponent<TransformComponent>(Vector2(350.f, 250.f), Vector2(1.f, 1.f));
	test.AddComponent<RigidBodyComponent>(Vector2(0.00f, 0.0f));
	test.AddComponent<SpriteComponent>("player1-test-image", Asset::DemoPlayer::ANIM_BACKWARDS, 2); // Only prints a default sprite
	test.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("player1-test-image") / 2, m_assetManager->GetSpriteHeight("player1-test-image"), Vector2());
	test.AddComponent<InputComponent>(Input::PlayerID::PLAYER_1, 18.f, 18.0f, 18.f, 18.f);
	test.AddComponent<AnimationComponent>(false, 8, true);
	test.Tag("Player1");
	test.Group("Player");

	// Create animation for test Player 1
	m_assetManager->CreateAnimation("player1-test-image", Asset::DemoPlayer::ANIM_BACKWARDS, 1.0f / 15.0f, { 0,1,2,3,4,5,6,7 });
	m_assetManager->CreateAnimation("player1-test-image", Asset::DemoPlayer::ANIM_LEFT, 1.0f / 15.0f, { 8,9,10,11,12,13,14,15 });
	m_assetManager->CreateAnimation("player1-test-image", Asset::DemoPlayer::ANIM_RIGHT, 1.0f / 15.0f, { 16,17,18,19,20,21,22,23 });
	m_assetManager->CreateAnimation("player1-test-image", Asset::DemoPlayer::ANIM_FORWARDS, 1.0f / 15.0f, { 24,25,26,27,28,29,30,31 });

	Entity test2 = m_coordinator->CreateEntity();
	test2.AddComponent<TransformComponent>(Vector2(450.f, 450.f), Vector2(1.f, 1.f));
	test2.AddComponent<RigidBodyComponent>(Vector2(-0.00f, 0.0f));
	test2.AddComponent<SpriteComponent>("player2-test-image", Asset::DemoPlayer::ANIM_FORWARDS, 2);
	test2.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("player2-test-image") / 4, m_assetManager->GetSpriteHeight("player2-test-image"), Vector2());
	test2.AddComponent<InputComponent>(Input::PlayerID::PLAYER_2, 18.f, 18.0f, 18.f, 18.f);
	test2.AddComponent<AnimationComponent>(false, 8, true);
	test2.Tag("Player2");
	test2.Group("Player");

	// Create animation for test Player 2
	m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_BACKWARDS, 1.0f / 15.0f, { 0,1,2,3,4,5,6,7 });
	m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_LEFT, 1.0f / 15.0f, { 8,9,10,11,12,13,14,15 });
	m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_RIGHT, 1.0f / 15.0f, { 16,17,18,19,20,21,22,23 });
	m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_FORWARDS, 1.0f / 15.0f, { 24,25,26,27,28,29,30,31 });

	// Add Key bindings for players
	// Player 1
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, 'W', Input::PlayerAction::MOVE_UP);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, 'D', Input::PlayerAction::MOVE_RIGHT);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, 'S', Input::PlayerAction::MOVE_DOWN);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, 'A', Input::PlayerAction::MOVE_LEFT);

	// Player 2
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, VK_UP, Input::PlayerAction::MOVE_UP);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, VK_RIGHT, Input::PlayerAction::MOVE_RIGHT);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, VK_DOWN, Input::PlayerAction::MOVE_DOWN);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, VK_LEFT, Input::PlayerAction::MOVE_LEFT);

	// Entity uiTextHello = m_coordinator->CreateEntity();
	// uiTextHello.AddComponent<UITextComponent>("New Render Text System!", Vector2(100, 100), Color(Colors::CYAN), FontType::HELVETICA_18);

	// Test
	Entity redBall = m_coordinator->CreateEntity();
	redBall.AddComponent<SpriteComponent>("red-ball");
	redBall.AddComponent<TransformComponent>(Vector2(450.f, 650.f), Vector2(1.f, 1.f));
	// Multiplying the acceleration with Physics::PIXEL_PER_METER 
	redBall.AddComponent<RigidBodyComponent>(Vector2(-200.0f, 0.0f), Vector2(), true);
	redBall.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("red-ball"), m_assetManager->GetSpriteHeight("red-ball"), Vector2());

	// Add Forces
	Vector2 wind = Vector2(2.0f * Physics::PIXEL_PER_METER, 0.0f);
	Vector2 gravity = Vector2(.0f, -9.8f * Physics::PIXEL_PER_METER);
	redBall.GetComponent<RigidBodyComponent>().AddForce(wind);
	redBall.GetComponent<RigidBodyComponent>().AddForce(gravity);
}

void TestApp::PrintTiles(const std::string& tileMapAssetId, float scale, const std::string& mapFileLocation, int rows, int cols)
{
	// Tiles positioning w.r.t each other
	float tileWidth = (m_assetManager->GetSprite(tileMapAssetId)->GetWidth()) * scale;
	float tileHeight = (m_assetManager->GetSprite(tileMapAssetId)->GetHeight()) * scale;
	float horizontalSpacing = tileWidth * 0.99f;
	float verticalSpacing = tileHeight * 0.25f;

	std::fstream mapFile;
	mapFile.open(mapFileLocation);

	for (int y = rows - 1; y >= 0; y--)
	{
		for (int x = 0; x < cols; x++)
		{
			// Store the integer and use the value as a frame number to print the correct tile
			int frame;
			mapFile >> frame;
			mapFile.ignore();

			// Calculate position
			float posX = static_cast<float>(x) * horizontalSpacing;
			float posY = static_cast<float>(y) * verticalSpacing;

			// Stagger even rows
			if (y % 2 == 0)
			{
				posX += tileWidth * 0.5f;
			}

			Entity tile = m_coordinator->CreateEntity();
			tile.Group("tiles");
			tile.AddComponent<TransformComponent>(Vector2(posX, posY), Vector2(scale, scale), 0.0f);

			// Set different components for different tiles
			switch (frame)
			{
				case Asset::Tiles::LOCK:
					// Higher z-index for Lock tiles
					tile.AddComponent<SpriteComponent>(tileMapAssetId, frame, 1);
					tile.AddComponent<BoxColliderComponent>(tileWidth, tileHeight, Vector2());
					break;
				default:
					tile.AddComponent<SpriteComponent>(tileMapAssetId, frame, 0);
					break;
			}
		}
	}
}

void TestApp::Update(float deltaTime)
{
	ProcessInput();
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
	m_coordinator->GetSystem<AnimationSystem>().Update(m_assetManager, deltaTime);
}

//------------------------------------------------------------------------
// Process key input for each player
//------------------------------------------------------------------------
void TestApp::ProcessInput()
{
	// For debug mode
	if (App::IsKeyPressed('B'))
	{
		bIsDebug = !bIsDebug;
	}
	ProcessPlayerKeys(Input::PlayerID::PLAYER_1, "Player1");
	ProcessPlayerKeys(Input::PlayerID::PLAYER_2, "Player2");
}

//------------------------------------------------------------------------
// Loop all player keys and check is any is currently clicked
//------------------------------------------------------------------------
void TestApp::ProcessPlayerKeys(Input::PlayerID playerId, const std::string& playerTag)
{
	// bool noKeyPressed = true;
	// For every keyState check if currently pressed and compare the new state(bIsPressed) with old state(bIsPressed)
	for (auto& keyState : m_inputManager->GetAllKeys(playerId))
	{
		if (App::IsKeyPressed(keyState.keyCode) != keyState.bIsPressed) // Key state is different
		{
			// store the current key state
			keyState.bIsPressed = !keyState.bIsPressed;

			m_inputManager->UpdateActionStatus(playerId, m_inputManager->GetActionEnum(playerId, keyState.keyCode), keyState.bIsPressed);

			const Input::PlayerAction action = m_inputManager->GetActionEnum(playerId, keyState.keyCode);
			const bool actionStatus = m_inputManager->GetActionStatus(playerId, action);
			m_eventManager->EmitEvent<ActionChangeEvent>(m_coordinator->GetEntityByTag(playerTag), playerId, action, actionStatus);
		}
	}
}

void TestApp::Render()
{
	// Update Render Systems
	m_coordinator->GetSystem<RenderSystem>().Update(m_assetManager);
	m_coordinator->GetSystem<RenderTextSystem>().Update();

	if (bIsDebug)
	{
		m_coordinator->GetSystem<RenderDebugSystem>().Update();

	}
}

void TestApp::Shutdown()
{
	Logger::Warn("TestApp::Shutdown()");
}
