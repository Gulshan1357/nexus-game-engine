#pragma once

#include <functional>

#include "IEvent.h"

class IEventCallback
{
public:
	virtual ~IEventCallback() = default;

	void Execute(IEvent& e)
	{
		Call(e);
	}
private:
	virtual void Call(IEvent& e) = 0;
};

template <typename TOwner, typename TEvent>
class EventCallback final : public IEventCallback
{
	// The Callback function is of the type TOwner::TEvent == Game::onCollisionEvent
	using CallbackFunction = std::function<void(TOwner*, TEvent&)>;

public:
	EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction)
		: m_ownerInstance(ownerInstance), m_callbackFunction(std::move(callbackFunction))
	{}

	virtual ~EventCallback() override = default;

private:
	TOwner* m_ownerInstance;
	CallbackFunction m_callbackFunction; // Function pointer

	virtual void Call(IEvent& e) override
	{
		m_callbackFunction(m_ownerInstance, static_cast<TEvent&>(e));
	}
};