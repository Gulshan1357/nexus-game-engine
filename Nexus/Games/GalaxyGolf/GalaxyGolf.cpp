#include "stdafx.h"
#include "GalaxyGolf.h"

#include <memory>
#include <utility>

#include "TerrainGenerator.h"
#include "Games/GameState.h"
#include "Games/Score.h"
#include "Games/UI/UIEffects.h"
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
#include "src/Systems/CameraFollowSystem.h"

#include "src/Systems/RenderSystem.h"
#include "src/Systems/CollisionSystem.h"
#include "src/Systems/RenderTextSystem.h"
#include "src/Systems/InputSystem.h"
#include "src/Systems/RenderDebugSystem.h"
#include "src/Systems/PhysicsSystem.h"
#include "src/Systems/AnimationSystem.h"
#include "src/Systems/ConstraintSystem.h"
#include "src/Systems/ParticleEffectSystem.h"
#include "src/Systems/RenderHUDSystem.h"

#include "src/Physics/Particle.h"
#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"

#include "src/Events/ActionChangeEvent.h"
#include "src/Systems/GameplaySystem.h"
#include "src/Systems/PlayerSystem.h"
#include "src/Systems/TrajectorySystem.h"
#include "src/Utils/Random.h"
#include "WorldSettings.h"

GalaxyGolf::GalaxyGolf(WorldType worldType, std::weak_ptr<GameState> gameState, std::weak_ptr<Score> score)
	: m_worldType(worldType), m_gameState(std::move(gameState)), m_score(std::move(score))
{
	m_isDebug = true;

	m_coordinator = std::make_unique<Coordinator>();
	m_eventManager = std::make_shared<EventManager>();
	m_inputManager = std::make_unique<InputManager>();
	m_assetManager = std::make_unique<AssetManager>();
	m_audioManager = std::make_shared<AudioManager>();
	Logger::Log("GalaxyGolf Game constructor called! ");
	Logger::Log("MapType: " + std::to_string(static_cast<int>(m_worldType)));

	if (auto gmState = m_gameState.lock())
	{
		Logger::Log("GameState is accessible.");
	}
	else
	{
		Logger::Err("GameState is no longer valid.");
	}

	if (auto sc = m_score.lock())
	{
		Logger::Log("Score is accessible.");
	}
	else
	{
		Logger::Err("Score is no longer valid.");
	}
}

GalaxyGolf::~GalaxyGolf()
{
	Logger::Warn("GalaxyGolf Game destructor called!");
}

void GalaxyGolf::Initialize()
{
	Logger::Log("GalaxyGolf::Initialize()");
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
	m_coordinator->AddSystem<RenderHUDSystem>();
	m_coordinator->AddSystem<GameplaySystem>(m_audioManager, m_gameState, m_score);
	m_coordinator->AddSystem<PlayerSystem>();
	m_coordinator->AddSystem<TrajectorySystem>();

	LoadLevel(1);

	// Before the main Update loop is started add the entities to the systems. So that system's entities are populate and we can call the InitializeEntityPhysics()
	m_coordinator->Update();
	// Calculate inverse mass, angular mass and inverse angular mass of entities with RigidBody
	m_coordinator->GetSystem<PhysicsSystem>().InitializeEntityPhysics();
	// Calculate the local coordinates of the connected entities in the joint w.r.t anchor(self entity)
	m_coordinator->GetSystem<ConstraintSystem>().InitializeLocalCoordinates();
}

void GalaxyGolf::LoadLevel(int level)
{
	// Sounds
	m_audioManager->AddAudio("golf_swing", R"(.\Assets\Audio\golf_swing.wav)");

	// Ground
	TerrainGenerator generator;
	TerrainGenerator::Config config = { 50.0f,
		 150.0f,
		-100.0f,
		100.0f,
		0.5f,
		 100.0f,
		700.0f,
		 20
	};
	terrainVertices = generator.GenerateTerrain(-300.f, 0.f, config);

	// Configure world settings
	switch (m_worldType)
	{
		case WorldType::EARTH:
		{
			Logger::Log("Earth selected!");
			m_worldSettings.gravity = -9.8f;
			m_worldSettings.windSpeed = Random::Float(-5.f, 5.f);
			m_worldSettings.atmosphereDrag = 0.01f;
			m_worldSettings.groundColor = Color(Colors::GRAY);
			AddTerrain(0.5f, 0.7f);
			break;
		}

		case WorldType::MARS:
		{
			Logger::Log("Mars selected!");
			m_worldSettings.gravity = -9.0f;
			m_worldSettings.windSpeed = Random::Float(-.5f, .5f);
			m_worldSettings.atmosphereDrag = 0.0005f;
			m_worldSettings.groundColor = Color(Colors::RED);
			AddTerrain(0.9f, 0.3f);
			break;
		}
		case WorldType::SUPER_EARTH:
		{
			Logger::Log("Super Earth selected!");
			m_worldSettings.gravity = -20.8f;
			m_worldSettings.windSpeed = Random::Float(-100.f, 100.f);
			m_worldSettings.atmosphereDrag = 0.03f;
			m_worldSettings.groundColor = Color(Colors::DARK_GRAY);
			AddTerrain(0.9f, 0.1f);
			break;
		}
	}

	// Add assets to the asset manager
	// m_assetManager->AddSprite("backgroundGrass", R"(.\Assets\Sprites\kenney_background\backgroundColorGrass.bmp)", 1, 1);
	m_assetManager->AddSprite("red-ball", R"(.\Assets\Sprites\ball_red_small.bmp)", 1, 1);
	m_assetManager->AddSprite("golf-ball", R"(.\Assets\Sprites\golf.bmp)", 1, 1);
	m_assetManager->AddSprite("hole", R"(.\Assets\Sprites\hole.bmp)", 1, 1);
	m_assetManager->AddSprite("flag", R"(.\Assets\Sprites\Flags\FlagRed.bmp)", 7, 1);

	// Animations
	m_assetManager->CreateAnimation("flag", 0, 1.0f / 15.0f, { 0,1,2,3,4,5,6 });

	// Entity background = m_coordinator->CreateEntity();
	// background.AddComponent<SpriteComponent>("backgroundGrass", 0);
	// background.AddComponent<TransformComponent>(Vector2(), Vector2(1.f, 1.f));
	// background.Tag("Background");

	// Red ball is the new Player 2
	Entity golfBall = m_coordinator->CreateEntity();
	golfBall.AddComponent<SpriteComponent>("golf-ball", 3);
	golfBall.AddComponent<TransformComponent>(Vector2(-100.f, 300.f), Vector2(0.5f, 0.5f), -0.3f);
	golfBall.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 5.f, 0.f, 0.0f, 1.f, 0.7f);
	golfBall.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	golfBall.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("golf-ball") / 4);
	golfBall.AddComponent<PlayerComponent>(Input::PlayerID::PLAYER_1);
	golfBall.AddComponent<CameraFollowComponent>();
	golfBall.Tag("Player1");
	golfBall.Group("Player");
	golfBall.AddComponent<ParticleEmitterComponent>(); // The type of emission is handled by Particle Effect system
	Logger::Log("Golf ball id = " + std::to_string(golfBall.GetId()));

	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, VK_LBUTTON, Input::PlayerAction::LMOUSE);

	// Entity hole = m_coordinator->CreateEntity();
	// hole.AddComponent<SpriteComponent>("hole", 3);
	// hole.AddComponent<TransformComponent>(Vector2(600.f, 0.f), Vector2(1.f, 1.f));
	// hole.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 0.f, 0.f, 0.0f, 0.1f, 0.1f);
	// hole.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	// hole.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("hole") / 2.f, Vector2(0.0f, m_assetManager->GetSpriteHeight("hole") / 1.5f));
	// hole.Tag("Hole");
	// Entity flag = m_coordinator->CreateEntity();
	// flag.AddComponent<TransformComponent>(Vector2(675.f, 45.f), Vector2(1.f, 1.f));
	// flag.AddComponent<SpriteComponent>("flag", 3);
	// flag.AddComponent<AnimationComponent>(true, 7, true);
	//
	// // Entity flag2 = m_coordinator->CreateEntity();
	// // flag2.AddComponent<TransformComponent>(Vector2(250.f, 45.f), Vector2(1.f, 1.f));
	// // flag2.AddComponent<SpriteComponent>("flag", 3);
	//
	// AddObstacleLaser(Vector2(250.f, 45.f), true);

}

void GalaxyGolf::Update(float deltaTime)
{

	ProcessInput();
	// TODO: For event system maybe find a more performant way to just subscribing the event once instead of resetting and subscribing over and over. Maybe a buffer of subscriptions that are only added and removed at certain "events" or for a certain object ID. Example, when an entity is removed, remove all the events associated with that entity.

	//------------------------------------------------------------------------
	// Reset all event callbacks for the current frame
	m_eventManager->Reset();

	//------------------------------------------------------------------------
	// Perform the subscription of the events for all systems
	m_coordinator->GetSystem<InputSystem>().SubscribeToEvents(m_eventManager);
	// m_coordinator->GetSystem<PhysicsSystem>().SubscribeToEvents(m_eventManager); // For collision resolution on collision
	m_coordinator->GetSystem<ConstraintSystem>().SubscribeToEvents(m_eventManager); // To clear the penetration vector and populate it on every collision
	m_coordinator->GetSystem<GameplaySystem>().SubscribeToEvents(m_eventManager);
	m_coordinator->GetSystem<ParticleEffectSystem>().SubscribeToEvents(m_eventManager); // Change particles based on the shot type
	//------------------------------------------------------------------------
	// Update the coordinator to process the entities that are waiting to be created/deleted
	m_coordinator->Update();

	//------------------------------------------------------------------------	
	// Invoke all the systems that needs to be updated
	m_coordinator->GetSystem<GameplaySystem>().Update();
	m_coordinator->GetSystem<AnimationSystem>().Update(m_assetManager, deltaTime);
	// [Physics system Start] Order is important. First integrate the forces, then resolve the constraint(penetration due to collision and joint), then integrate the velocities
	const float dt = deltaTime / 1000.0f; // Converting to seconds
	m_coordinator->GetSystem<PhysicsSystem>().UpdateForces(dt, m_worldSettings);
	m_coordinator->GetSystem<CollisionSystem>().Update(m_eventManager);
	m_coordinator->GetSystem<ConstraintSystem>().Update(dt);
	m_coordinator->GetSystem<PhysicsSystem>().UpdateVelocities(dt);
	// [Physics system End]
	m_coordinator->GetSystem<ParticleEffectSystem>().Update(dt);
	m_coordinator->GetSystem<CameraFollowSystem>().Update(m_camera);
	m_coordinator->GetSystem<PlayerSystem>().Update(m_eventManager);
	m_coordinator->GetSystem<TrajectorySystem>().Update(dt); // If left click hold then store mouse position for trajectory calculations


	// Move background w.r.t camera for parallax effect.
	// m_coordinator->GetEntityByTag("Background").GetComponent<TransformComponent>().position = m_camera.GetPosition();
}

void GalaxyGolf::ProcessInput()
{
	// For debug mode
	if (App::IsKeyPressed('B'))
	{
		m_isDebug = !m_isDebug;
	}

	ProcessPlayerKeys(Input::PlayerID::PLAYER_1, "Player1");
}

void GalaxyGolf::ProcessPlayerKeys(Input::PlayerID playerId, const std::string& playerTag)
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
			try
			{
				m_eventManager->EmitEvent<ActionChangeEvent>(m_coordinator->GetEntityByTag(playerTag), playerId, action, actionStatus);
			}
			catch ([[maybe_unused]] const std::out_of_range& e)
			{
				// Do
				Logger::Err("ProcessPlayerKeys(): Couldn't find the entity with player tag");
			}

		}
	}
}

void GalaxyGolf::Render()
{
	// Background
	UIEffects::RenderFadingBackground(Color(Colors::BG_DARK_BLUE), 2.5f);
	UIEffects::RenderStartField(Color(Colors::WHITE), 100, 12345);

	// Render Terrain
	TerrainGenerator::Render(m_camera, terrainVertices, m_worldSettings.groundColor);

	// Update Render Systems
	m_coordinator->GetSystem<RenderSystem>().Update(m_assetManager, m_camera);
	m_coordinator->GetSystem<ParticleEffectSystem>().Render(m_camera);
	m_coordinator->GetSystem<RenderTextSystem>().Update(m_camera);
	m_coordinator->GetSystem<TrajectorySystem>().Render(m_camera);

	if (m_isDebug)
	{
		m_coordinator->GetSystem<RenderDebugSystem>().Update(m_camera);
	}

	m_coordinator->GetSystem<RenderHUDSystem>().Update(m_camera, m_worldType, m_worldSettings, Color(Colors::WHITE));
}

void GalaxyGolf::Shutdown()
{
	Logger::Warn("GalaxyGolf::Shutdown()");
}

void GalaxyGolf::AddTerrain(const float elasticity, const float friction)
{
	for (size_t i = 0; i < terrainVertices.size() - 1; i++)
	{
		const Vector2& currentPos = terrainVertices[i];
		const Vector2& nextPos = terrainVertices[i + 1];
		float xDifference = nextPos.x - currentPos.x;
		float yDifference = nextPos.y - currentPos.y;
		float colliderDepth = 10.f;

		// Terrain entity will be a point with polygon collider stretching till the next point
		Entity terrain = m_coordinator->CreateEntity();
		terrain.AddComponent<TransformComponent>(currentPos, Vector2(1.f, 1.f));

		// Add collider type
		terrain.AddComponent<ColliderTypeComponent>(ColliderType::Polygon);
		std::vector<Vector2> polygonVertices;
		polygonVertices.emplace_back(0.f, -colliderDepth); // Bottom left
		polygonVertices.emplace_back(xDifference, yDifference - colliderDepth);    // Bottom right
		polygonVertices.emplace_back(xDifference, yDifference);    // Top right
		polygonVertices.emplace_back(0.f, 0.f); // Top left
		terrain.AddComponent<PolygonColliderComponent>(polygonVertices);
		terrain.AddComponent<RigidBodyComponent>(Vector2(), Vector2(), false, 0.0f, 0.0f, 0.0f, elasticity, friction);
		terrain.Group("Terrain");

		// Adding a circle collider between 2 points because the collision resolution b/w circle and polygon vertex is not stable
		// Entity terrainConnector = m_coordinator->CreateEntity();
		// terrainConnector.AddComponent<TransformComponent>(nextPos, Vector2(1.f, 1.f));
		// terrainConnector.AddComponent<RigidBodyComponent>(Vector2(), Vector2(), false, 0.0f, 0.0f, 0.0f, elasticity, friction);
		// terrainConnector.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
		// terrainConnector.AddComponent<CircleColliderComponent>(1.f);
		// terrainConnector.Group("Terrain");
	}
}

void GalaxyGolf::AddObstacleLaser(Vector2 position, bool isHorizontal)
{
	// Add sprites for the laser and laser shooter
	m_assetManager->AddSprite("laser", R"(.\Assets\Sprites\Obstacles\laser.bmp)", 1, 1);
	m_assetManager->AddSprite("laser_shooter", R"(.\Assets\Sprites\Obstacles\laser_shooter.bmp)", 1, 1);

	// Create the laser entity
	Entity laser = m_coordinator->CreateEntity();
	laser.AddComponent<SpriteComponent>("laser", 3);
	laser.AddComponent<TransformComponent>(
		position,
		Vector2(1.f, 1.f),
		isHorizontal ? PI * 0.5f : 0.0f
	);
	laser.AddComponent<RigidBodyComponent>(
		Vector2(0.0f, 0.0f), Vector2(), false, 0.f, 0.f, 0.0f, 0.1f, 0.1f
	);
	laser.AddComponent<CircleColliderComponent>(
		m_assetManager->GetSpriteWidth("laser") / 2.f,
		Vector2(0.0f, m_assetManager->GetSpriteHeight("laser") / 1.5f)
	);
	laser.Tag("Laser");
	laser.Group("Killers");
	laser.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	if (isHorizontal) laser.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("laser"), m_assetManager->GetSpriteHeight("laser") * 0.9f);
	else laser.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("laser"), m_assetManager->GetSpriteHeight("laser") * 0.9f);
	Logger::Log("Killers ball id = " + std::to_string(laser.GetId()));

	// Create the first laser shooter entity (top or left)
	Entity shooter1 = m_coordinator->CreateEntity();
	shooter1.AddComponent<SpriteComponent>("laser_shooter", 3);
	shooter1.AddComponent<TransformComponent>(
		isHorizontal
		? Vector2(position.x - m_assetManager->GetSpriteHeight("laser") * 0.6f, position.y)
		: Vector2(position.x, position.y - m_assetManager->GetSpriteHeight("laser") * 0.6f),
		Vector2(1.f, 1.f), // Scale
		isHorizontal ? -PI * 0.5f : 0 // Rotation
	);
	shooter1.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	shooter1.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteHeight("laser_shooter") * 0.42f);
	shooter1.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 0.f, 0.f, 0.0f, 0.1f, 0.1f);

	shooter1.Tag("LaserShooter");

	// Create the second laser shooter entity (bottom or right)
	Entity shooter2 = m_coordinator->CreateEntity();
	shooter2.AddComponent<SpriteComponent>("laser_shooter", 3);
	shooter2.AddComponent<TransformComponent>(
		isHorizontal
		? Vector2(position.x + m_assetManager->GetSpriteHeight("laser") * 0.6f, position.y)
		: Vector2(position.x, position.y + m_assetManager->GetSpriteHeight("laser") * 0.6f),
		Vector2(1.f, 1.f),
		static_cast<float>(isHorizontal ? PI * 0.5 : PI) // Rotation
	);
	shooter2.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	shooter2.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteHeight("laser_shooter") * 0.42f);
	shooter2.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 0.f, 0.f, 0.0f, 0.1f, 0.1f);
	shooter2.Tag("LaserShooter");

}

// void GalaxyGolf::SpawnShape(Vector2 position, ColliderType colliderType) const
// {
// 	// Polygon local vertices for red-ball sprite
// 	std::vector<Vector2> polygonVertices;
// 	// Going clockwise from top left
// 	polygonVertices.emplace_back(-(m_assetManager->GetSpriteWidth("red-ball") / 2.f), -(m_assetManager->GetSpriteHeight("red-ball") / 2.f));
// 	polygonVertices.emplace_back(0.0f, -(m_assetManager->GetSpriteHeight("red-ball")));
// 	polygonVertices.emplace_back((m_assetManager->GetSpriteWidth("red-ball") / 2.f), -(m_assetManager->GetSpriteHeight("red-ball") / 2.f));
// 	polygonVertices.emplace_back((m_assetManager->GetSpriteWidth("red-ball") / 2.f), (m_assetManager->GetSpriteHeight("red-ball") / 2.f));
// 	polygonVertices.emplace_back(0.0f, (m_assetManager->GetSpriteHeight("red-ball")));
// 	polygonVertices.emplace_back(-(m_assetManager->GetSpriteWidth("red-ball") / 2.f), (m_assetManager->GetSpriteHeight("red-ball") / 2.f));
//
// 	Entity shape = m_coordinator->CreateEntity();
// 	shape.AddComponent<SpriteComponent>("golf-ball", 3);
// 	shape.AddComponent<TransformComponent>(position, Vector2(1.f, 1.f));
// 	shape.AddComponent<RigidBodyComponent>(Vector2(-00.0f, 1.0f), Vector2(), false, 5.0f, 0.0f, 0.0f, 0.3f, 0.1f);
// 	shape.Tag("SpawnItem");
// 	switch (colliderType)
// 	{
// 		case ColliderType::Box:
// 			shape.AddComponent<ColliderTypeComponent>(ColliderType::Box);
// 			shape.AddComponent<BoxColliderComponent>(m_assetManager->GetSpriteWidth("red-ball"), m_assetManager->GetSpriteHeight("red-ball") * 3, Vector2());
// 			break;
// 		case ColliderType::Polygon:
// 			shape.AddComponent<ColliderTypeComponent>(ColliderType::Polygon);
// 			shape.AddComponent<PolygonColliderComponent>(polygonVertices);
// 			break;
// 		case ColliderType::Circle:
// 			shape.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
// 			shape.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("red-ball") / 2);
// 			break;
// 	}
// 	// We need to calculate the moment of Inertia, inverse mass etc. for the spawned entities
// 	PhysicsEngine::InitializeEntityPhysics(shape);
// }
