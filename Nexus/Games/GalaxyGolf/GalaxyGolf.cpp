#include "stdafx.h"
#include "GalaxyGolf.h"

#include <memory>

#include "MapType.h"
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
#include "src/Components/InputComponent.h"
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

#include "src/Physics/Particle.h"
#include "src/Utils/Vector2.h"
#include "src/Utils/Logger.h"

#include "src/Events/ActionChangeEvent.h"

GalaxyGolf::GalaxyGolf(MapType mapType)
{
	m_isDebug = true;

	m_mapType = mapType;

	m_coordinator = std::make_unique<Coordinator>();
	m_eventManager = std::make_unique<EventManager>();
	m_inputManager = std::make_unique<InputManager>();
	m_assetManager = std::make_unique<AssetManager>();
	m_audioManager = std::make_unique<AudioManager>();
	Logger::Log("Game constructor called! " + std::to_string(static_cast<int>(m_mapType)));
}

GalaxyGolf::~GalaxyGolf()
{
	Logger::Warn("Game destructor called!");
}

void GalaxyGolf::Initialize(MapType mapType)
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

void GalaxyGolf::LoadLevel(int level)
{
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

	// Walls and grounds
	Entity ground = m_coordinator->CreateEntity();
	ground.AddComponent<TransformComponent>(Vector2(static_cast<float>(Physics::SCREEN_WIDTH) / 2, -240.0f), Vector2(1.f, 1.f));
	ground.AddComponent<RigidBodyComponent>(Vector2(), Vector2(), false, 0.0f, 0.0f, 0.0f, 0.3f, 0.7f);
	ground.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	ground.AddComponent<BoxColliderComponent>(static_cast<float>(Physics::SCREEN_WIDTH) - 10, 500.0f);
	ground.Tag("ground");

	Entity uiTextHello = m_coordinator->CreateEntity();
	uiTextHello.AddComponent<UITextComponent>("Render Text System!", Vector2(100, 100), Color(Colors::CYAN), FontType::HELVETICA_18, true);


}

void GalaxyGolf::Update(float deltaTime)
{
	Logger
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
}

void GalaxyGolf::ProcessInput()
{
	if (App::IsKeyPressed('B'))
	{
		m_isDebug = !m_isDebug;
	}
}

void GalaxyGolf::Render()
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

void GalaxyGolf::Shutdown()
{
	Logger::Warn("TestApp::Shutdown()");
}
