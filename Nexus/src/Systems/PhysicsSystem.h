#pragma once

#include <memory>
#include <functional>

#include "src/ECS/System.h"

#include "src/EventManagement/EventManager.h"
#include "src/Events/CollisionEvent.h"

#include "src/Components/BoxColliderComponent.h"

class PhysicsSystem : public System
{
public:
	PhysicsSystem()
	{
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(const std::unique_ptr<EventManager>& eventManager)
	{
		using CallbackType = std::function<void(PhysicsSystem*, CollisionEvent&)>;
		const CallbackType callback = [this](auto&&, auto&& placeholder2) { onCollision(std::forward<decltype(placeholder2)>(placeholder2)); };
		eventManager->SubscribeToEvent<CollisionEvent>(this, callback);
		//  std::placeholders::_2 tells std::bind that the second argument (the event) will be provided when the resulting function is called. This allows us to create a callable object that matches the required function signature of SubscribeToEvent, where the first argument is the instance (DamageSystem*), and the second argument is the event (CollisionEvent&).
	}

	void onCollision(const CollisionEvent& event)
	{
		// Logger::Log("Physics system received an event collision between entities " + std::to_string(event.a.GetId()) + " and " + std::to_string(event.b.GetId()));

		// Drag on collision with water tiles
		if (event.a.HasTag("Player2"))
		{
			event.a.GetComponent<RigidBodyComponent>().velocity = PhysicsEngine::GenerateDragForce(event.a.GetComponent<RigidBodyComponent>(), 0.001f);
		}
		else if (event.b.HasTag("Player2"))
		{
			event.b.GetComponent<RigidBodyComponent>().velocity = PhysicsEngine::GenerateDragForce(event.b.GetComponent<RigidBodyComponent>(), 0.001f);
		}
	}
};