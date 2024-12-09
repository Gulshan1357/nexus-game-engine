#pragma once

#include <memory>

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"
#include "src/Event/EventBus.h"

#include "src/Events/KeyPressEvent.h"
#include "src/Input/InputEnums.h"

#include "src/Components/InputComponent.h"
#include "src/Components/RigidbodyComponent.h"

#include "src/Utils/Logger.h"

//------------------------------------------------------------------------
// The input system listens to Input events (Keyboard key press) and perform necessary action.
// ------------------------------------------------------------------------
class InputSystem : public System
{
public:
	InputSystem()
	{
		RequireComponent<InputComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus)
	{
		using CallbackType = std::function<void(InputSystem*, KeyPressEvent&)>;
		CallbackType callback = std::bind(&InputSystem::OnKeyPressed, this, std::placeholders::_2);
		eventBus->SubscribeToEvent<KeyPressEvent>(this, callback);
		// std::bind usually copies the so need std::ref to ensure object is passed by reference wrapper using std::ref
	}

	void OnKeyPressed(const KeyPressEvent& event)
	{
		if (event.action == Input::PlayerAction::MOVE_UP)
		{
			PerformMoveUpAction(event.playerId, event.player);
		}
		if (event.action == Input::PlayerAction::MOVE_RIGHT)
		{
			PerformMoveRightAction(event.playerId, event.player);
		}
		if (event.action == Input::PlayerAction::MOVE_DOWN)
		{
			PerformMoveDownAction(event.playerId, event.player);
		}
		if (event.action == Input::PlayerAction::MOVE_LEFT)
		{
			PerformMoveLeftAction(event.playerId, event.player);
		}
		if (event.action == Input::PlayerAction::JUMP)
		{
			PerformJump(event.playerId, event.player);
		}
	}

private:
	//------------------------------------------------------------------------
	// Definition of all the actions. Action can be tailored to each player using playerID
	// ------------------------------------------------------------------------
	void PerformMoveUpAction(Input::PlayerID playerId, Entity player)
	{
		const auto inputComponent = player.GetComponent<InputComponent>();
		auto& rigidbody = player.GetComponent<RigidBodyComponent>();

		rigidbody.velocity += inputComponent.upVelocity;
	}

	void PerformMoveRightAction(Input::PlayerID playerId, Entity player)
	{
		const auto inputComponent = player.GetComponent<InputComponent>();
		auto& rigidbody = player.GetComponent<RigidBodyComponent>();

		rigidbody.velocity += inputComponent.rightVelocity;
	}

	void PerformMoveDownAction(Input::PlayerID playerId, Entity player)
	{
		const auto inputComponent = player.GetComponent<InputComponent>();
		auto& rigidbody = player.GetComponent<RigidBodyComponent>();

		rigidbody.velocity += inputComponent.downVelocity;
	}

	void PerformMoveLeftAction(Input::PlayerID playerId, Entity player)
	{
		const auto inputComponent = player.GetComponent<InputComponent>();
		auto& rigidbody = player.GetComponent<RigidBodyComponent>();

		rigidbody.velocity += inputComponent.leftVelocity;
	}

	void PerformJump(Input::PlayerID playerId, Entity player)
	{
		const auto inputComponent = player.GetComponent<InputComponent>();
		auto& rigidbody = player.GetComponent<RigidBodyComponent>();

		rigidbody.velocity += inputComponent.upVelocity * 2;
	}
};