#include "stdafx.h"
#include "GalaxyGolf.h"

#include <memory>
#include <utility>

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
#include "src/PCG/PCG.h"

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
	Shutdown();
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
	m_coordinator->AddSystem<GameplaySystem>(m_coordinator, m_assetManager, m_audioManager, m_gameState, m_score);
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
	m_audioManager->AddAudio("GameplayBG", R"(.\Assets\Audio\chiphead64.wav)");
	m_audioManager->AddAudio("golf_swing", R"(.\Assets\Audio\golf_swing.wav)");
	m_audioManager->AddAudio("explosion", R"(.\Assets\Audio\Explosion.wav)");
	m_audioManager->AddAudio("wood-impact", R"(.\Assets\Audio\Wood_crash.wav)");
	m_audioManager->AddAudio("stone-impact", R"(.\Assets\Audio\stone_impact.wav)");

	m_audioManager->PlayAudio("GameplayBG", true);

	// Configure world settings
	switch (m_worldType)
	{
		case WorldType::EARTH:
		{
			Logger::Log("Earth selected!");
			m_worldSettings.gravity = -9.8f;
			m_worldSettings.windSpeed = Random::Float(-5.f, 5.f);
			m_worldSettings.atmosphereDrag = 0.01f;
			m_worldSettings.groundColor = Color(Colors::NEON_GREEN);
			m_terrainVertices = PCG::GenerateLevel(m_coordinator, m_assetManager, { 700.f, 20, 0.5f, 0.7f });
			break;
		}

		case WorldType::MARS:
		{
			Logger::Log("Mars selected!");
			m_worldSettings.gravity = -9.5f;
			m_worldSettings.windSpeed = Random::Float(-.5f, .5f);
			m_worldSettings.atmosphereDrag = 0.0008f;
			m_worldSettings.groundColor = Color(Colors::NEON_RED);
			m_terrainVertices = PCG::GenerateLevel(m_coordinator, m_assetManager, { 700.f, 20, 0.7f, 0.3f });
			break;
		}
		case WorldType::SUPER_EARTH:
		{
			Logger::Log("Super Earth selected!");
			m_worldSettings.gravity = -15.8f;
			m_worldSettings.windSpeed = Random::Float(-100.f, 100.f);
			m_worldSettings.atmosphereDrag = 0.03f;
			m_worldSettings.groundColor = Color(Colors::NEON_BLUE);
			m_terrainVertices = PCG::GenerateLevel(m_coordinator, m_assetManager, { 700.f, 20, 0.6f, 0.1f });
			break;
		}
	}

	// Add assets to the asset manager
	// m_assetManager->AddSprite("backgroundGrass", R"(.\Assets\Sprites\kenney_background\backgroundColorGrass.bmp)", 1, 1);
	m_assetManager->AddSprite("red-ball", R"(.\Assets\Sprites\ball_red_small.bmp)", 1, 1);
	m_assetManager->AddSprite("golf-ball", R"(.\Assets\Sprites\golf.bmp)", 1, 1);
	m_assetManager->AddSprite("alien", R"(.\Assets\Sprites\kenney_physics-assets\Aliens\alienPink_round.bmp)", 1, 1);

	// Animations
	m_assetManager->CreateAnimation("flag", 0, 1.0f / 15.0f, { 0,1,2,3,4,5,6 });

	// Entity background = m_coordinator->CreateEntity();
	// background.AddComponent<SpriteComponent>("backgroundGrass", 0);
	// background.AddComponent<TransformComponent>(Vector2(), Vector2(1.f, 1.f));
	// background.Tag("Background");

	// Red ball is the new Player 2
	Entity playerOne = m_coordinator->CreateEntity();
	playerOne.AddComponent<SpriteComponent>("golf-ball", 3);
	playerOne.AddComponent<TransformComponent>(Vector2(-100.f, 300.f), Vector2(0.5f, 0.5f), -0.3f);
	playerOne.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 10.f, 0.f, 0.0f, 1.f, 0.7f);
	playerOne.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	playerOne.AddComponent<CircleColliderComponent>(m_assetManager->GetSpriteWidth("golf-ball") / 4);
	playerOne.AddComponent<PlayerComponent>(Input::PlayerID::PLAYER_1);
	playerOne.AddComponent<CameraFollowComponent>();
	playerOne.Tag("Player1");
	playerOne.Group("Player");
	playerOne.AddComponent<ParticleEmitterComponent>(); // The type of emission is handled by Particle Effect system
	Logger::Log("Golf ball id = " + std::to_string(playerOne.GetId()));

	m_inputManager->AddInputKeyToAction(Input::PlayerID::PLAYER_1, VK_LBUTTON, Input::PlayerAction::LMOUSE);

	Entity alienOne = m_coordinator->CreateEntity();
	alienOne.AddComponent<SpriteComponent>("alien", 3);
	alienOne.AddComponent<TransformComponent>(Vector2(100.f, 300.f), Vector2(0.5f, 0.5f), -0.3f);
	alienOne.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 5.f, 0.f, 0.0f, 1.f, 0.7f);
	alienOne.Group("Aliens");
	alienOne.AddRelationship(playerOne, "Attracted");

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
	m_coordinator->GetSystem<GameplaySystem>().Update(m_terrainVertices);
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
	PCG::RenderTerrain(m_camera, m_terrainVertices, m_worldSettings.groundColor);

	// Update RenderTerrain Systems
	m_coordinator->GetSystem<RenderSystem>().Update(m_assetManager, m_camera);
	m_coordinator->GetSystem<ParticleEffectSystem>().Render(m_camera);
	m_coordinator->GetSystem<RenderTextSystem>().Update(m_camera);
	m_coordinator->GetSystem<TrajectorySystem>().Render(m_camera);

	if (m_isDebug)
	{
		m_coordinator->GetSystem<RenderDebugSystem>().Render(m_camera);
	}
	m_coordinator->GetSystem<RenderDebugSystem>().RenderConnectedEntites(m_camera);

	m_coordinator->GetSystem<RenderHUDSystem>().Update(m_camera, m_worldType, m_worldSettings, Color(Colors::WHITE));
	m_coordinator->GetSystem<InputSystem>().RenderForce(m_camera);
}

void GalaxyGolf::Shutdown()
{
	if (m_audioManager->IsAudioPlaying("GameplayBG"))
	{
		m_audioManager->StopAudio("GameplayBG");
		m_audioManager->ClearAudioMap();
	}
	
	Logger::Warn("GalaxyGolf::Shutdown()");
}
