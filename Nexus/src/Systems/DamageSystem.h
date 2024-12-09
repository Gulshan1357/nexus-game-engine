#pragma once

#include <memory>
#include <functional>

#include "src/ECS/System.h"

#include "src/Components/BoxColliderComponent.h"

#include "src/Event/EventBus.h"
#include "src/Events/CollisionEvent.h"

#include "src/Utils/Logger.h"

class DamageSystem : public System
{
public:
	DamageSystem()
	{
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus)
	{
		using CallbackType = std::function<void(DamageSystem*, CollisionEvent&)>;
		CallbackType callback = std::bind(&DamageSystem::onCollision, this, std::placeholders::_2);
		eventBus->SubscribeToEvent<CollisionEvent>(this, callback);
		//  std::placeholders::_2 tells std::bind that the second argument (the event) will be provided when the resulting function is called. This allows us to create a callable object that matches the required function signature of SubscribeToEvent, where the first argument is the instance (DamageSystem*), and the second argument is the event (CollisionEvent&).
	}

	void onCollision(const CollisionEvent& event)
	{
		Logger::Log("Damage system received an event collision between entities " + std::to_string(event.a.GetId()) + " and " + std::to_string(event.b.GetId()));
		//event.a.Kill();
		//event.b.Kill();

		/*auto entities = GetSystemEntities();
		std::cout << "onCollision Entities size: = " + std::to_string(entities.size()) + " \n";*/


	}
};