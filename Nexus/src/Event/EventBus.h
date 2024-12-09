#pragma once

#include <functional>
#include <map>
#include <typeindex>
#include <memory>
#include <list>

#include "EventCallback.h"

#include <src/Utils/Logger.h>

class EventBus
{
private:
	using CallbackList = std::list<std::unique_ptr<IEventCallback>>;

public:
	EventBus()
	{
		Logger::Log("EventBus constructor called!");
	}

	~EventBus()
	{
		Logger::Log("EventBus destructor called!");
	}

	// Clears the subscribers list
	void Reset()
	{
		m_subscribers.clear();
	}

	//------------------------------------------------------------------------
	// Subscribe to an event type <T>
	// A listener subscribe to an event
	// Example : eventBus->SubscribeToEvent<CollisionEvent>(this, &System::OnCollision);
	// ------------------------------------------------------------------------
	template <typename TEvent, typename TOwner>
	void SubscribeToEvent(TOwner* ownerInstance, std::function<void(TOwner*, TEvent&)> callbackFunction)
	{
		if (!m_subscribers[typeid(TEvent)].get())
		{
			m_subscribers[typeid(TEvent)] = std::make_unique<CallbackList>();
		}
		auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);

		// Since the subscriber is a unique pointer, we need to use std::move when we want to push it to a map. We use std::move to change the ownership of an object from one unique_ptr to another unique_ptr.
		m_subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
	}

	//------------------------------------------------------------------------
	// Emit an event type <T>
	// As soon as something emits an event the game is blocked until all the listener callbacks are executed first
	// Example: eventBus->EmitEvent<CollisionEvent>(player, enemy);
	// Bug: Try to always pass the concerned entities with events, The callback function are unable to retrieve system entities using GetSystemEntities().
	//------------------------------------------------------------------------
	template<typename TEvent, typename ...TArgs>
	void EmitEvent(TArgs&& ...args)
	{

		if (const auto eventCallbacks = m_subscribers[typeid(TEvent)].get())
		{
			for (auto& callback : *eventCallbacks)
			{
				TEvent event(std::forward<TArgs>(args)...);
				callback->Execute(event);
			}
		}
		else
		{
			Logger::Warn("No callbacks for event type!");
		}
	}

private:
	//------------------------------------------------------------------------
	// This map will contain event as key and a list of callback functions as Value
	// Example: <Key, Value> = <CollisionEvent, [subscriberCallback*, subscriberCallback*]
	//------------------------------------------------------------------------
	std::map<std::type_index, std::unique_ptr<CallbackList>> m_subscribers;

	// TODO: Compare the performance of std::map vs std::unordered_map for subscribers. Decided to go with std::map because of cache friendliness as it is contiguous. But std::unordered_map has an average search complexity of O(1) compared to O(log N) for std::map.
};