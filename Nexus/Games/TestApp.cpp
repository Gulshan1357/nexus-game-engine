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
#include "src/Components/UITextComponent.h"
#include "src/Components/InputComponent.h"
#include "src/Components/ColliderTypeComponent.h"
#include "src/Components/BoxColliderComponent.h"
#include "src/Components/CircleColliderComponent.h"
#include "src/Components/PolygonColliderComponent.h"

#include "src/Systems/RenderSystem.h"
#include "src/Systems/CollisionSystem.h"
#include "src/Systems/DamageSystem.h"
#include "src/Systems/RenderTextSystem.h"
#include "src/Systems/InputSystem.h"
#include "src/Systems/RenderDebugSystem.h"
#include "src/Systems/PhysicsSystem.h"
#include "src/Systems/AnimationSystem.h"

#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"

#include "src/Components/AnimationComponent.h"

#include "src/Events/ActionChangeEvent.h"

TestApp::TestApp()
{
	bIsDebug = true;

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


	// Before the main Update loop is started add the entities to the systems. So that system's entities are populate and we can call the InitializeEntityPhysics()
	m_coordinator->Update();
	// Calculate inverse mass, angular mass and inverse angular mass of entities with RigidBody
	m_coordinator->GetSystem<PhysicsSystem>().InitializeEntityPhysics();
}

void TestApp::LoadLevel(int level)
{
	// Add Systems
	m_coordinator->AddSystem<RenderSystem>();
	m_coordinator->AddSystem<CollisionSystem>();
	m_coordinator->AddSystem<DamageSystem>();
	m_coordinator->AddSystem<RenderTextSystem>();
	m_coordinator->AddSystem<InputSystem>();
	m_coordinator->AddSystem<RenderDebugSystem>();
	m_coordinator->AddSystem<AnimationSystem>();
	m_coordinator->AddSystem<PhysicsSystem>();

	// Add assets to the asset manager
	m_assetManager->AddSprite("player1-test-image", R"(.\Assets\Sprites\Test.bmp)", 8, 4);
	m_assetManager->AddSprite("player2-test-image", R"(.\Assets\Sprites\Test.bmp)", 8, 4);
	m_assetManager->AddSprite("tile-map", R"(.\Assets\Sprites\tilesheet.bmp)", 21, 8);
	m_assetManager->AddSprite("blue-ball", R"(.\Assets\Sprites\ball_blue_small.bmp)", 1, 1);
	m_assetManager->AddSprite("blue-ball-large", R"(.\Assets\Sprites\ball_blue_large.bmp)", 1, 1);
	m_assetManager->AddSprite("red-ball", R"(.\Assets\Sprites\ball_red_small.bmp)", 1, 1);
	m_assetManager->AddSprite("red-ball-large", R"(.\Assets\Sprites\ball_red_large.bmp)", 1, 1);

	// Print TileMaps
	PrintTiles("tile-map", static_cast<float>(0.7), R"(.\Assets\Sprites\test2.map)", 20, 20);

	// Animation SystemLimitation: If one player is animating then all should animation component otherwise different player sprite start animating on different player's input
	// Add Entities and Components
	Entity test = m_coordinator->CreateEntity();
	test.AddComponent<TransformComponent>(Vector2(350.f, 250.f), Vector2(1.f, 1.f));
	test.AddComponent<RigidBodyComponent>(Vector2(0.00f, 0.0f));
	test.AddComponent<SpriteComponent>("player1-test-image", 2, Asset::DemoPlayer::ANIM_BACKWARDS); // Only prints a default sprite
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

	// Entity test2 = m_coordinator->CreateEntity();
	// test2.AddComponent<TransformComponent>(Vector2(450.f, 450.f), Vector2(1.f, 1.f));
	// test2.AddComponent<RigidBodyComponent>(Vector2(-0.00f, 0.0f));
	// test2.AddComponent<SpriteComponent>("player2-test-image", 2, Asset::DemoPlayer::ANIM_FORWARDS);
	// test2.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("player2-test-image") / 4, m_assetManager->GetSpriteHeight("player2-test-image"), Vector2());
	// test2.AddComponent<InputComponent>(Input::PlayerID::PLAYER_2, 18.f, 18.0f, 18.f, 18.f);
	// test2.Tag("Player2");
	// test2.Group("Player");

	// // Create animation for test Player 2
	// m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_BACKWARDS, 1.0f / 15.0f, { 0,1,2,3,4,5,6,7 });
	// m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_LEFT, 1.0f / 15.0f, { 8,9,10,11,12,13,14,15 });
	// m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_RIGHT, 1.0f / 15.0f, { 16,17,18,19,20,21,22,23 });
	// m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_FORWARDS, 1.0f / 15.0f, { 24,25,26,27,28,29,30,31 });
	//
	// Add Key bindings for player 1
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, VK_UP, Input::PlayerAction::MOVE_UP);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, VK_RIGHT, Input::PlayerAction::MOVE_RIGHT);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, VK_DOWN, Input::PlayerAction::MOVE_DOWN);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, VK_LEFT, Input::PlayerAction::MOVE_LEFT);

	// Entity uiTextHello = m_coordinator->CreateEntity();
	// uiTextHello.AddComponent<UITextComponent>("New Render Text System!", Vector2(100, 100), Color(Colors::CYAN), FontType::HELVETICA_18);

	// Polygon local vertices for red-ball sprite
	std::vector<Vector2> redballPolygonVertices;
	// Going clockwise from top left
	redballPolygonVertices.emplace_back(-(m_assetManager->GetSpriteWidth("red-ball") / 2.f), -(m_assetManager->GetSpriteHeight("red-ball") / 2.f));
	redballPolygonVertices.emplace_back((m_assetManager->GetSpriteWidth("red-ball") / 2.f), -(m_assetManager->GetSpriteHeight("red-ball") / 2.f));
	redballPolygonVertices.emplace_back((m_assetManager->GetSpriteWidth("red-ball") / 2.f), (m_assetManager->GetSpriteHeight("red-ball") / 2.f));
	redballPolygonVertices.emplace_back(-(m_assetManager->GetSpriteWidth("red-ball") / 2.f), (m_assetManager->GetSpriteHeight("red-ball") / 2.f));

	// Red ball is the new Player 2
	Entity redBall = m_coordinator->CreateEntity();
	redBall.AddComponent<SpriteComponent>("red-ball", 3);
	redBall.AddComponent<TransformComponent>(Vector2(200.f, 100.f), Vector2(1.f, 1.f), 0.03f);
	redBall.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), true, 10.f, 0.f, 0.0f, 0.1f);
	redBall.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	// redBall.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("red-ball"));
	redBall.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("red-ball"), m_assetManager->GetSpriteHeight("red-ball"), Vector2());
	// redBall.AddComponent<PolygonColliderComponent>(redballPolygonVertices);
	redBall.AddComponent<InputComponent>(Input::PlayerID::PLAYER_2, 2018.f, 2018.0f, 2018.f, 2018.f);
	redBall.Tag("Player2");
	redBall.Group("Player");

	Entity redBigBall = m_coordinator->CreateEntity();
	redBigBall.AddComponent<SpriteComponent>("red-ball", 3);
	redBigBall.AddComponent<TransformComponent>(Vector2(200.5f, 300.f), Vector2(1.f, 1.f));
	redBigBall.AddComponent<RigidBodyComponent>(Vector2(-00.0f, 0.0f), Vector2(), true, 10.0f, 0.0f, 0.0f, 0.0f, 0.1f);
	redBigBall.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	redBigBall.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("red-ball"), m_assetManager->GetSpriteHeight("red-ball"), Vector2());
	// redBigBall.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("red-ball") / 2);
	// redBigBall.AddComponent<PolygonColliderComponent>(redballPolygonVertices);

	redBigBall.Tag("BigBall");

	// Add Key bindings for player 2
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, 'W', Input::PlayerAction::MOVE_UP);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, 'D', Input::PlayerAction::MOVE_RIGHT);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, 'S', Input::PlayerAction::MOVE_DOWN);
	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, 'A', Input::PlayerAction::MOVE_LEFT);

	// Entity ball6 = m_coordinator->CreateEntity();
	// ball6.AddComponent<SpriteComponent>("red-ball", 3);
	// ball6.AddComponent<TransformComponent>(Vector2(100.f, 100.f), Vector2(1.f, 1.f), 0.13f);
	// ball6.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), true, 50.f, 0.f);
	// ball6.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	// // ball6.AddComponent<PolygonColliderComponent>(redballPolygonVertices);
	// /*ball6.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("red-ball"), m_assetManager->GetSpriteHeight("red-ball"));*/
	// ball6.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("red-ball") / 2);
	//
	// // Adding Torque
	// float torque = 260;
	// ball6.GetComponent<RigidBodyComponent>().AddTorque(torque);
	// ball6.Tag("Debug");

	// Adding weight force
	// Vector2 weight = Vector2(0.0f, redBigBall.GetComponent<RigidBodyComponent>().mass * -9.8f * Physics::PIXEL_PER_METER);
	// redBigBall.GetComponent<RigidBodyComponent>().AddForce(weight);

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
				case Asset::Tiles::GRASS:
					tile.AddComponent<SpriteComponent>(tileMapAssetId, 0, Asset::Tiles::GRASS);
					break;
				case Asset::Tiles::DESERT:
					tile.AddComponent<SpriteComponent>(tileMapAssetId, 0, Asset::Tiles::DESERT);
					break;
				case Asset::Tiles::WATER:
					tile.AddComponent<SpriteComponent>(tileMapAssetId, 0, Asset::Tiles::WATER);
					// tile.AddComponent<BoxColliderComponent>(tileWidth / 4, tileHeight / 4, Vector2());
					break;
				case Asset::Tiles::LAVA:
					tile.AddComponent<SpriteComponent>(tileMapAssetId, 0, Asset::Tiles::LAVA);
					break;
				case Asset::Tiles::LOCK:
					// Higher z-index for Lock tiles
					tile.AddComponent<SpriteComponent>(tileMapAssetId, 1, Asset::Tiles::LOCK);
					// tile.AddComponent<BoxColliderComponent>(tileWidth, tileHeight, Vector2());
					break;
				case Asset::Tiles::KEY:
					tile.AddComponent<SpriteComponent>(tileMapAssetId, 0, Asset::Tiles::KEY);
					break;
				default:
					tile.AddComponent<SpriteComponent>(tileMapAssetId, 0, Asset::Tiles::GRASS);
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
	m_coordinator->GetSystem<PhysicsSystem>().SubscribeToEvents(m_eventManager); // For collision resolution on collision

	// Update the coordinator to process the entities that are waiting to be created/deleted
	m_coordinator->Update();

	// Invoke all the systems that needs to be updated
	m_coordinator->GetSystem<PhysicsSystem>().Update(deltaTime);
	m_coordinator->GetSystem<CollisionSystem>().Update(m_eventManager);
	m_coordinator->GetSystem<AnimationSystem>().Update(m_assetManager, deltaTime);
	m_coordinator->GetSystem<PhysicsSystem>().Update(deltaTime);
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
