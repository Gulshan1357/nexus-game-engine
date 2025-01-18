#include "stdafx.h"
#include "GalaxyGolf.h"

#include <memory>
#include <utility>

#include "MapType.h"
#include "Games/GameState.h"
#include "Games/Score.h"
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
#include "src/Systems/DamageSystem.h"
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

GalaxyGolf::GalaxyGolf(MapType mapType, std::weak_ptr<GameState> gameState, std::weak_ptr<Score> score)
	: m_mapType(mapType), m_gameState(std::move(gameState)), m_score(std::move(score))
{
	m_isDebug = true;

	m_coordinator = std::make_unique<Coordinator>();
	m_eventManager = std::make_unique<EventManager>();
	m_inputManager = std::make_unique<InputManager>();
	m_assetManager = std::make_unique<AssetManager>();
	m_audioManager = std::make_unique<AudioManager>();
	Logger::Log("GalaxyGolf Game constructor called! ");
	Logger::Log("MapType: " + std::to_string(static_cast<int>(m_mapType)));

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
	m_coordinator->AddSystem<DamageSystem>();
	m_coordinator->AddSystem<RenderTextSystem>();
	m_coordinator->AddSystem<InputSystem>();
	m_coordinator->AddSystem<RenderDebugSystem>();
	m_coordinator->AddSystem<AnimationSystem>();
	m_coordinator->AddSystem<PhysicsSystem>();
	m_coordinator->AddSystem<ConstraintSystem>();
	m_coordinator->AddSystem<ParticleEffectSystem>();
	m_coordinator->AddSystem<CameraFollowSystem>();
	m_coordinator->AddSystem<RenderHUDSystem>();
	m_coordinator->AddSystem<GameplaySystem>();
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
	// Walls and grounds
	Entity ground = m_coordinator->CreateEntity();
	ground.AddComponent<TransformComponent>(Vector2(static_cast<float>(Physics::SCREEN_WIDTH) / 2, -240.0f), Vector2(1.f, 1.f));
	ground.AddComponent<RigidBodyComponent>(Vector2(), Vector2(), false, 0.0f, 0.0f, 0.0f, 0.3f, 0.7f);
	ground.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	ground.AddComponent<BoxColliderComponent>(static_cast<float>(Physics::SCREEN_WIDTH) * 10, 500.0f);
	ground.Tag("ground");

	// Add assets to the asset manager
	m_assetManager->AddSprite("red-ball", R"(.\Assets\Sprites\ball_red_small.bmp)", 1, 1);
	m_assetManager->AddSprite("hole", R"(.\Assets\Sprites\hole.bmp)", 1, 1);

	// Red ball is the new Player 2
	Entity redBall = m_coordinator->CreateEntity();
	redBall.AddComponent<SpriteComponent>("red-ball", 3);
	redBall.AddComponent<TransformComponent>(Vector2(400.f, 300.f), Vector2(1.f, 1.f), -0.3f);
	redBall.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 5.f, 0.f, 0.0f, 0.1f, 0.1f);
	redBall.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	redBall.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("red-ball") / 2.f);
	redBall.AddComponent<PlayerComponent>(Input::PlayerID::PLAYER_1);
	redBall.AddComponent<CameraFollowComponent>();
	redBall.Tag("Player1");
	redBall.Group("Player");
	redBall.AddComponent<ParticleEmitterComponent>(); // The type of emission is handled by Particle Effect system

	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, VK_LBUTTON, Input::PlayerAction::LMOUSE);

	Entity hole = m_coordinator->CreateEntity();
	hole.AddComponent<SpriteComponent>("hole", 3);
	hole.AddComponent<TransformComponent>(Vector2(600.f, 0.f), Vector2(1.f, 1.f));
	hole.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 0.f, 0.f, 0.0f, 0.1f, 0.1f);
	hole.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	hole.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("hole") / 2.f, Vector2(0.0f, m_assetManager->GetSpriteHeight("hole") / 1.5f));
	hole.Tag("Hole");
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
	m_coordinator->GetSystem<DamageSystem>().SubscribeToEvents(m_eventManager);
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
	m_coordinator->GetSystem<AnimationSystem>().Update(m_assetManager, deltaTime);
	// [Physics system Start] Order is important. First integrate the forces, then resolve the constraint(penetration due to collision and joint), then integrate the velocities
	const float dt = deltaTime / 1000.0f; // Converting to seconds
	m_coordinator->GetSystem<PhysicsSystem>().UpdateForces(dt);
	m_coordinator->GetSystem<CollisionSystem>().Update(m_eventManager);
	m_coordinator->GetSystem<ConstraintSystem>().Update(dt);
	m_coordinator->GetSystem<PhysicsSystem>().UpdateVelocities(dt);
	// [Physics system End]
	m_coordinator->GetSystem<ParticleEffectSystem>().Update(dt);
	m_coordinator->GetSystem<CameraFollowSystem>().Update(m_camera);
	m_coordinator->GetSystem<PlayerSystem>().Update(m_eventManager);
	m_coordinator->GetSystem<TrajectorySystem>().Update(dt); // If left click hold then store mouse position for trajectory calculations
}

void GalaxyGolf::ProcessInput()
{
	if (App::IsKeyPressed('B'))
	{
		m_isDebug = !m_isDebug;
	}

	if (App::IsKeyPressed('P'))
	{
		if (auto gmState = m_gameState.lock())
		{
			Logger::Log("GalaxyGolf::ProcessInput() GameState is accessible.");
			*gmState = GameState::GAME_OVER;
		}
		else
		{
			Logger::Err("GalaxyGolf::ProcessInput() GameState is no longer valid.");
		}
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
			m_eventManager->EmitEvent<ActionChangeEvent>(m_coordinator->GetEntityByTag(playerTag), playerId, action, actionStatus);
		}
	}
}

void GalaxyGolf::PropagateScore()
{
	if (auto sc = m_score.lock())
	{
		// Update Score based on the player component so that the GameOverScreen can display it
		sc->playerOneTotalShots = m_coordinator->GetEntityByTag("Player1").GetComponent<PlayerComponent>().totalStrokes;
		// Logger::Warn(std::to_string(sc->playerOneTotalShots));
	}
	else
	{
		Logger::Err("Score is no longer valid.");
	}
}

void GalaxyGolf::Render()
{
	// Update Render Systems
	m_coordinator->GetSystem<RenderSystem>().Update(m_assetManager, m_camera);
	m_coordinator->GetSystem<ParticleEffectSystem>().Render(m_camera);
	m_coordinator->GetSystem<RenderTextSystem>().Update(m_camera);
	m_coordinator->GetSystem<TrajectorySystem>().Render(m_camera);

	if (m_isDebug)
	{
		m_coordinator->GetSystem<RenderDebugSystem>().Update(m_camera);
	}

	m_coordinator->GetSystem<RenderHUDSystem>().Update(m_camera);
}

void GalaxyGolf::Shutdown()
{
	PropagateScore();
	Logger::Warn("GalaxyGolf::Shutdown()");
}
