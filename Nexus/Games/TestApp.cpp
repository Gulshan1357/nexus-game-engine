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
#include "src/AudioManagement/AudioManager.h"

#include "src/Components/TransformComponent.h"
#include "src/Components/SpriteComponent.h"
#include "src/Components/UITextComponent.h"
#include "src/Components/PlayerComponent.h"
#include "src/Components/ColliderTypeComponent.h"
#include "src/Components/BoxColliderComponent.h"
#include "src/Components/CircleColliderComponent.h"
#include "src/Components/PolygonColliderComponent.h"
#include "src/Components/AnimationComponent.h"
#include "src/Components/CameraFollowComponent.h"
#include "src/Components/ConstraintTypeComponent.h"
#include "src/Components/ParticleEmitterComponent.h"

#include "src/Systems/RenderSystem.h"
#include "src/Systems/CollisionSystem.h"
#include "src/Systems/RenderTextSystem.h"
#include "src/Systems/InputSystem.h"
#include "src/Systems/RenderDebugSystem.h"
#include "src/Systems/PhysicsSystem.h"
#include "src/Systems/AnimationSystem.h"
#include "src/Systems/ConstraintSystem.h"
#include "src/Systems/ParticleEffectSystem.h"

#include "src/Physics/Particle.h"
#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"

#include "src/Events/ActionChangeEvent.h"
#include "src/Systems/CameraFollowSystem.h"

TestApp::TestApp()
{
	m_isDebug = true;

	m_coordinator = std::make_unique<Coordinator>();
	m_eventManager = std::make_unique<EventManager>();
	m_inputManager = std::make_unique<InputManager>();
	m_assetManager = std::make_unique<AssetManager>();
	m_audioManager = std::make_unique<AudioManager>();
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
	// Calculate the local coordinates of the connected entities in the joint w.r.t anchor(self entity)
	m_coordinator->GetSystem<ConstraintSystem>().InitializeLocalCoordinates();
}

void TestApp::LoadLevel(int level)
{
	// Camera
	m_camera.SetPosition(Physics::SCREEN_WIDTH / 2.f, Physics::SCREEN_HEIGHT / 2.f);
	// Add Systems
	m_coordinator->AddSystem<RenderSystem>();
	m_coordinator->AddSystem<CollisionSystem>();
	m_coordinator->AddSystem<RenderTextSystem>();
	m_coordinator->AddSystem<InputSystem>();
	m_coordinator->AddSystem<RenderDebugSystem>();
	m_coordinator->AddSystem<AnimationSystem>();
	m_coordinator->AddSystem<PhysicsSystem>();
	m_coordinator->AddSystem<ConstraintSystem>();
	m_coordinator->AddSystem<ParticleEffectSystem>();
	m_coordinator->AddSystem<CameraFollowSystem>();

	// Add assets to the asset manager
	m_assetManager->AddSprite("player1-test-image", R"(.\Assets\Sprites\Test.bmp)", 8, 4);
	m_assetManager->AddSprite("player2-test-image", R"(.\Assets\Sprites\Test.bmp)", 8, 4);
	m_assetManager->AddSprite("tile-map", R"(.\Assets\Sprites\tilesheet.bmp)", 21, 8);
	m_assetManager->AddSprite("blue-ball", R"(.\Assets\Sprites\ball_blue_small.bmp)", 1, 1);
	m_assetManager->AddSprite("blue-ball-large", R"(.\Assets\Sprites\ball_blue_large.bmp)", 1, 1);
	m_assetManager->AddSprite("red-ball", R"(.\Assets\Sprites\ball_red_small.bmp)", 1, 1);
	m_assetManager->AddSprite("red-ball-large", R"(.\Assets\Sprites\ball_red_large.bmp)", 1, 1);

	// Add sounds to the audio manager
	m_audioManager->AddAudio("gun-shot", R"(.\Assets\Audio\weapon_gun_shot_008.wav)");
	m_audioManager->AddAudio("cannon-shot", R"(.\Assets\Audio\weapon_cannon_shot_002.wav)");

	// Print TileMaps
	PrintTiles("tile-map", static_cast<float>(0.7), R"(.\Assets\Sprites\test2.map)", 20, 20);

	// Walls and grounds
	Entity ground = m_coordinator->CreateEntity();
	ground.AddComponent<TransformComponent>(Vector2(static_cast<float>(Physics::SCREEN_WIDTH) / 2, -240.0f), Vector2(1.f, 1.f));
	ground.AddComponent<RigidBodyComponent>(Vector2(), Vector2(), false, 0.0f, 0.0f, 0.0f, 0.3f, 0.7f);
	ground.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	ground.AddComponent<BoxColliderComponent>(static_cast<float>(Physics::SCREEN_WIDTH) - 10, 500.0f);
	ground.Tag("ground");
	// Entity leftWall = m_coordinator->CreateEntity();
	// leftWall.AddComponent<TransformComponent>(Vector2(0.f, static_cast<float>(Physics::SCREEN_HEIGHT) / 2), Vector2(1.f, 1.f));
	// leftWall.AddComponent<RigidBodyComponent>(Vector2(), Vector2(), false, 0.0f);
	// leftWall.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	// leftWall.AddComponent<BoxColliderComponent>(5.f, static_cast<float>(Physics::SCREEN_HEIGHT));
	Entity rightWall = m_coordinator->CreateEntity();
	rightWall.AddComponent<TransformComponent>(Vector2(static_cast<float>(Physics::SCREEN_WIDTH), static_cast<float>(Physics::SCREEN_HEIGHT) / 2), Vector2(1.f, 1.f));
	rightWall.AddComponent<RigidBodyComponent>(Vector2(), Vector2(), false, 0.0f);
	rightWall.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	rightWall.AddComponent<BoxColliderComponent>(5.f, static_cast<float>(Physics::SCREEN_HEIGHT));
	// Entity ceiling = m_coordinator->CreateEntity();
	// ceiling.AddComponent<TransformComponent>(Vector2(static_cast<float>(Physics::SCREEN_WIDTH)/2, static_cast<float>(Physics::SCREEN_HEIGHT)), Vector2(1.f, 1.f));
	// ceiling.AddComponent<RigidBodyComponent>(Vector2(), Vector2(), false, 0.0f);
	// ceiling.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	// ceiling.AddComponent<BoxColliderComponent>(static_cast<float>(Physics::SCREEN_WIDTH), 5.0f);

	// Animation SystemLimitation: If one player is animating then all should animation component otherwise different player sprite start animating on different player's input
	// Add Entities and Components
	Entity test = m_coordinator->CreateEntity();
	test.AddComponent<TransformComponent>(Vector2(350.f, 250.f), Vector2(1.f, 1.f));
	test.AddComponent<RigidBodyComponent>(Vector2(0.00f, 0.0f), Vector2(), false, 0.0f);
	test.AddComponent<SpriteComponent>("player1-test-image", 2, Asset::DemoPlayer::ANIM_BACKWARDS); // Only prints a default sprite
	// test.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	// test.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("player1-test-image") / 2, m_assetManager->GetSpriteHeight("player1-test-image"), Vector2());
	test.AddComponent<PlayerComponent>(Input::PlayerID::PLAYER_1);
	test.AddComponent<AnimationComponent>(false, 8, true);
	// test.AddComponent<CameraFollowComponent>();
	test.Tag("Player1");
	test.Group("Player");

	// Create animation for test Player 1
	m_assetManager->CreateAnimation("player1-test-image", Asset::DemoPlayer::ANIM_BACKWARDS, 1.0f / 15.0f, { 0,1,2,3,4,5,6,7 });
	m_assetManager->CreateAnimation("player1-test-image", Asset::DemoPlayer::ANIM_LEFT, 1.0f / 15.0f, { 8,9,10,11,12,13,14,15 });
	m_assetManager->CreateAnimation("player1-test-image", Asset::DemoPlayer::ANIM_RIGHT, 1.0f / 15.0f, { 16,17,18,19,20,21,22,23 });
	m_assetManager->CreateAnimation("player1-test-image", Asset::DemoPlayer::ANIM_FORWARDS, 1.0f / 15.0f, { 24,25,26,27,28,29,30,31 });

	// Entity test2 = m_coordinator->CreateEntity();
	// test2.AddComponent<TransformComponent>(Vector2(450.f, 450.f), Vector2(1.f, 1.f));
	// test2.AddComponent<RigidBodyComponent>(Vector2(-0.00f, 0.0f), false, 1.0f);
	// test2.AddComponent<SpriteComponent>("player2-test-image", 2, Asset::DemoPlayer::ANIM_FORWARDS);
	// test2.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("player2-test-image") / 4, m_assetManager->GetSpriteHeight("player2-test-image"), Vector2());
	// test2.AddComponent<PlayerComponent>(Input::PlayerID::PLAYER_2, 18.f, 18.0f, 18.f, 18.f);
	// test2.Tag("Player2");
	// test2.Group("Player");

	// Create animation for test Player 2
	m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_BACKWARDS, 1.0f / 15.0f, { 0,1,2,3,4,5,6,7 });
	m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_LEFT, 1.0f / 15.0f, { 8,9,10,11,12,13,14,15 });
	m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_RIGHT, 1.0f / 15.0f, { 16,17,18,19,20,21,22,23 });
	m_assetManager->CreateAnimation("player2-test-image", Asset::DemoPlayer::ANIM_FORWARDS, 1.0f / 15.0f, { 24,25,26,27,28,29,30,31 });

	// Add Key bindings for player 1
	// m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, VK_UP, Input::PlayerAction::MOVE_UP);
	// m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, VK_RIGHT, Input::PlayerAction::MOVE_RIGHT);
	// m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, VK_DOWN, Input::PlayerAction::MOVE_DOWN);
	// m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, VK_LEFT, Input::PlayerAction::MOVE_LEFT);

	Entity uiTextHello = m_coordinator->CreateEntity();
	uiTextHello.AddComponent<UITextComponent>("Render Text System!", Vector2(100, 100), Color(Colors::CYAN), FontType::HELVETICA_18, true);

	// Polygon local vertices for red-ball sprite
	std::vector<Vector2> redballPolygonVertices;
	// Going clockwise from top left
	redballPolygonVertices.emplace_back(-(m_assetManager->GetSpriteWidth("red-ball") / 2.f), -(m_assetManager->GetSpriteHeight("red-ball") / 2.f));
	redballPolygonVertices.emplace_back(0.0f, -(m_assetManager->GetSpriteHeight("red-ball")));
	redballPolygonVertices.emplace_back((m_assetManager->GetSpriteWidth("red-ball") / 2.f), -(m_assetManager->GetSpriteHeight("red-ball") / 2.f));
	redballPolygonVertices.emplace_back((m_assetManager->GetSpriteWidth("red-ball") / 2.f), (m_assetManager->GetSpriteHeight("red-ball") / 2.f));
	redballPolygonVertices.emplace_back(0.0f, (m_assetManager->GetSpriteHeight("red-ball")));
	redballPolygonVertices.emplace_back(-(m_assetManager->GetSpriteWidth("red-ball") / 2.f), (m_assetManager->GetSpriteHeight("red-ball") / 2.f));

	// Red ball is the new Player 2
	Entity redBall = m_coordinator->CreateEntity();
	redBall.AddComponent<SpriteComponent>("red-ball", 3);
	redBall.AddComponent<TransformComponent>(Vector2(400.f, 300.f), Vector2(1.f, 1.f), -0.3f);
	redBall.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 5.f, 0.f, 0.0f, 0.1f, 0.1f);
	redBall.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	redBall.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("red-ball") / 2.f);
	// redBall.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("red-ball"), m_assetManager->GetSpriteHeight("red-ball"), Vector2());
	// redBall.AddComponent<PolygonColliderComponent>(redballPolygonVertices);
	redBall.AddComponent<PlayerComponent>(Input::PlayerID::PLAYER_2);
	redBall.AddComponent<CameraFollowComponent>();
	redBall.Tag("Player2");
	redBall.Group("Player");

	// Add Key bindings for player 2
	// m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, 'W', Input::PlayerAction::MOVE_UP);
	// m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, 'D', Input::PlayerAction::MOVE_RIGHT);
	// m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, 'S', Input::PlayerAction::MOVE_DOWN);
	// m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_2, 'A', Input::PlayerAction::MOVE_LEFT);

	// // Constraints
	// std::vector<Entity> joinedEntities;
	// for (size_t i = 0; i < 8; i++)
	// {
	// 	Entity ball = m_coordinator->CreateEntity();
	// 	float x = Physics::SCREEN_WIDTH / 2.0f - (static_cast<float>(i) * 60.f);
	// 	ball.AddComponent<TransformComponent>(Vector2(x, 750.f), Vector2(1.f, 1.f));
	// 	ball.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, static_cast<float>(i), 0.f, 0.0f, 0.1f, 0.1f);
	// 	ball.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	// 	// ballA.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("red-ball") * 4);
	// 	ball.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("red-ball"), m_assetManager->GetSpriteHeight("red-ball"), Vector2());
	// 	ball.AddComponent<SpriteComponent>("red-ball");
	// 	joinedEntities.push_back(ball);
	// }
	//
	// for (size_t i = 0; i < joinedEntities.size() - 1; i++)
	// {
	// 	Vector2 midpoint = (joinedEntities[i].GetComponent<TransformComponent>().position + joinedEntities[i + 1].GetComponent<TransformComponent>().position) * 0.5f;
	// 	Entity joint = m_coordinator->CreateEntity();
	// 	joint.AddComponent<TransformComponent>(midpoint, Vector2(0.1f, 0.1f));
	// 	joint.AddComponent<SpriteComponent>("blue-ball");
	// 	joint.AddComponent<ConstraintTypeComponent>(ConstrainType::JOINT);
	// 	joint.AddComponent<JointConstraintComponent>(joinedEntities[i], joinedEntities[i + 1]);
	// }

	// Smoke Effect
	ParticleProps smokeProps;
	smokeProps.colorBegin = Color(0.7f, 0.7f, 0.7f);   // Light gray
	smokeProps.colorEnd = Color(0.4f, 0.4f, 0.4f);     // Darker gray, fading out
	smokeProps.sizeBegin = 10.0f;                   // Start larger
	smokeProps.sizeEnd = 30.0f;                     // Grow as it rises
	smokeProps.sizeVariations = 5.0f;               // More size variation
	smokeProps.lifeTime = 4.0f;                     // Longer lifetime
	smokeProps.velocity = { 0.0f, 50.0f };            // Slow upward movement (positive Y)
	smokeProps.velocityVariations = { 20.f, 10.f };   // Slight horizontal drift
	smokeProps.position = { 0.0f, 0.0f };

	// Fountain Effect
	ParticleProps fountainProps;
	fountainProps.particleShape = ParticleShape::CIRCLE;
	fountainProps.colorBegin = Color(0.25f, 0.64f, 0.87f);   // Light blue
	fountainProps.colorEnd = Color(0.12f, 0.4f, 0.69f);      // Darker blue
	fountainProps.sizeBegin = 8.0f;
	fountainProps.sizeEnd = 4.0f;                     // Shrink as they fall
	fountainProps.sizeVariations = 2.0f;
	fountainProps.lifeTime = 4.0f;
	fountainProps.velocity = { 0.0f, 300.0f };          // Strong upward velocity (positive Y)
	fountainProps.velocityVariations = { 100.f, 50.f }; // More horizontal spread
	fountainProps.position = { 0.0f, 0.0f };
	fountainProps.useGravity = true;
	fountainProps.gravityStrength = 50.0f;

	redBall.AddComponent<ParticleEmitterComponent>(
		fountainProps,
		15.f,
		true,
		EmissionShape::POINT,
		15.0f,
		0.0f
	);

	test.AddComponent<ParticleEmitterComponent>(
		smokeProps,
		15.f,
		true,
		EmissionShape::CIRCLE,
		15.0f,
		0.0f
	);

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
	// Vector2 position = m_coordinator->GetEntityByTag("Player2").GetComponent<TransformComponent>().position;
	// m_camera.SetPosition(position);
	ProcessInput();
	// TODO: For event system maybe find a more performant way to just subscribing the event once instead of resetting and subscribing over and over. Maybe a buffer of subscriptions that are only added and removed at certain "events" or for a certain object ID. Example, when an entity is removed, remove all the events associated with that entity.

	//------------------------------------------------------------------------
	// Reset all event callbacks for the current frame
	m_eventManager->Reset();

	//------------------------------------------------------------------------
	// Perform the subscription of the events for all systems
	// m_coordinator->GetSystem<InputSystem>().SubscribeToEvents(m_eventManager);
	// m_coordinator->GetSystem<PhysicsSystem>().SubscribeToEvents(m_eventManager); // For collision resolution on collision
	// m_coordinator->GetSystem<ConstraintSystem>().SubscribeToEvents(m_eventManager); // To clear the penetration vector and populate it on every collision

	//------------------------------------------------------------------------
	// Update the coordinator to process the entities that are waiting to be created/deleted
	m_coordinator->Update();

	//------------------------------------------------------------------------	
	// Invoke all the systems that needs to be updated
	m_coordinator->GetSystem<AnimationSystem>().Update(m_assetManager, deltaTime);
	// [Physics system Start] Order is important. First integrate the forces, then resolve the constraint(penetration due to collision and joint), then integrate the velocities
	const float dt = deltaTime / 1000.0f; // Converting to seconds
	// m_coordinator->GetSystem<PhysicsSystem>().UpdateForces(dt);
	// m_coordinator->GetSystem<CollisionSystem>().Update(m_eventManager);
	m_coordinator->GetSystem<ConstraintSystem>().Update(dt);
	m_coordinator->GetSystem<PhysicsSystem>().UpdateVelocities(dt);
	// [Physics system End]
	m_coordinator->GetSystem<ParticleEffectSystem>().Update(dt);
	m_coordinator->GetSystem<CameraFollowSystem>().Update(m_camera);
}

//------------------------------------------------------------------------
// Process key input for each player
//------------------------------------------------------------------------
void TestApp::ProcessInput()
{
	// For debug mode
	Vector2 mousePos;
	App::GetMousePos(mousePos.x, mousePos.y);

	if (App::IsKeyPressed('B'))
	{
		m_isDebug = !m_isDebug;
	}
	if ((App::IsKeyPressed(VK_LBUTTON) == true) && m_bWasLMousePressedPast == false)
	{
		Logger::Warn("Left mouse button pressed");
		SpawnShape(mousePos, ColliderType::Polygon);
	}
	if ((App::IsKeyPressed(VK_RBUTTON) == true))
	{
		Logger::Warn("Right mouse button pressed");
		m_audioManager->PlayAudio("gun-shot");
	}

	// m_coordinator->GetEntityByTag("SpawnItem").GetComponent<TransformComponent>().position = mousePos;
	// m_coordinator->GetEntityByTag("SpawnItem").GetComponent<TransformComponent>().rotation = .2f;
	m_bWasLMousePressedPast = App::IsKeyPressed(VK_LBUTTON);
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

void TestApp::SpawnShape(Vector2 position, ColliderType colliderType) const
{
	// Polygon local vertices for red-ball sprite
	std::vector<Vector2> polygonVertices;
	// Going clockwise from top left
	polygonVertices.emplace_back(-(m_assetManager->GetSpriteWidth("red-ball") / 2.f), -(m_assetManager->GetSpriteHeight("red-ball") / 2.f));
	polygonVertices.emplace_back(0.0f, -(m_assetManager->GetSpriteHeight("red-ball")));
	polygonVertices.emplace_back((m_assetManager->GetSpriteWidth("red-ball") / 2.f), -(m_assetManager->GetSpriteHeight("red-ball") / 2.f));
	polygonVertices.emplace_back((m_assetManager->GetSpriteWidth("red-ball") / 2.f), (m_assetManager->GetSpriteHeight("red-ball") / 2.f));
	polygonVertices.emplace_back(0.0f, (m_assetManager->GetSpriteHeight("red-ball")));
	polygonVertices.emplace_back(-(m_assetManager->GetSpriteWidth("red-ball") / 2.f), (m_assetManager->GetSpriteHeight("red-ball") / 2.f));

	Entity shape = m_coordinator->CreateEntity();
	shape.AddComponent<SpriteComponent>("red-ball", 3);
	shape.AddComponent<TransformComponent>(position, Vector2(1.f, 1.f));
	shape.AddComponent<RigidBodyComponent>(Vector2(-00.0f, 1.0f), Vector2(), false, 5.0f, 0.0f, 0.0f, 0.3f, 0.1f);
	shape.Tag("SpawnItem");
	switch (colliderType)
	{
		case ColliderType::Box:
			shape.AddComponent<ColliderTypeComponent>(ColliderType::Box);
			shape.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("red-ball") * 3, m_assetManager->GetSpriteHeight("red-ball") * 3, Vector2());
			break;
		case ColliderType::Polygon:
			shape.AddComponent<ColliderTypeComponent>(ColliderType::Polygon);
			shape.AddComponent<PolygonColliderComponent>(polygonVertices);
			break;
		case ColliderType::Circle:
			shape.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
			shape.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("red-ball") / 2);
			break;
	}
	// We need to calculate the moment of Inertia, inverse mass etc. for the spawned entities
	PhysicsEngine::InitializeEntityPhysics(shape);
}

void TestApp::Render()
{
	// Update Render Systems
	m_coordinator->GetSystem<RenderSystem>().Update(m_assetManager, m_camera);
	m_coordinator->GetSystem<ParticleEffectSystem>().Render(m_camera);
	m_coordinator->GetSystem<RenderTextSystem>().Update(m_camera);

	if (m_isDebug)
	{
		m_coordinator->GetSystem<RenderDebugSystem>().Update(m_camera);
	}
}

void TestApp::Shutdown()
{
	Logger::Warn("TestApp::Shutdown()");
}
