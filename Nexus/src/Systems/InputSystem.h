#pragma once

#include <memory>

#include "src/Components/AnimationComponent.h"
#include "src/ECS/System.h"
#include "src/ECS/Entity.h"
#include "src/EventManagement/EventManager.h"

#include "src/Events/ActionChangeEvent.h"
#include "src/InputManagement/InputEnums.h"

#include "src/Components/InputComponent.h"
#include "src/Components/RigidBodyComponent.h"

//------------------------------------------------------------------------
// The input system listens to Input events (Keyboard key press) and perform necessary action.
// ------------------------------------------------------------------------
class InputSystem : public System
{
public:
	InputSystem()
	{
		RequireComponent<InputComponent>();
		RequireComponent<AnimationComponent>();
	}

	void SubscribeToEvents(const std::unique_ptr<EventManager>& eventManager)
	{
		using CallbackType = std::function<void(InputSystem*, ActionChangeEvent&)>;
		const CallbackType callback = [this](auto&&, auto&& placeholder2) { OnActionChange(std::forward<decltype(placeholder2)>(placeholder2)); };
		eventManager->SubscribeToEvent<ActionChangeEvent>(this, callback);
		// std::bind usually copies the so need std::ref to ensure object is passed by reference wrapper using std::ref
	}

	void OnActionChange(const ActionChangeEvent& actionEvent)
	{
		// Currently we are sending events when action gets activated and also when it gets deactivated.
		// TODO: Improve the control flow. Example: what happens when both MOVE_UP and MOVE_RIGHT are active?
		if (actionEvent.action == Input::PlayerAction::MOVE_UP && actionEvent.isActionActive)
		{
			PerformMoveUp(actionEvent.playerId, actionEvent.player);
		}
		else if (actionEvent.action == Input::PlayerAction::MOVE_RIGHT && actionEvent.isActionActive)
		{
			PerformMoveRight(actionEvent.playerId, actionEvent.player);
		}
		else if (actionEvent.action == Input::PlayerAction::MOVE_DOWN && actionEvent.isActionActive)
		{
			PerformMoveDown(actionEvent.playerId, actionEvent.player);
		}
		else if (actionEvent.action == Input::PlayerAction::MOVE_LEFT && actionEvent.isActionActive)
		{
			PerformMoveLeft(actionEvent.playerId, actionEvent.player);
		}
		else if (actionEvent.action == Input::PlayerAction::JUMP && actionEvent.isActionActive)
		{
			PerformJump(actionEvent.playerId, actionEvent.player);
		}
		// If none of the above then stop movement and animation
		else
		{
			PerformIdle(actionEvent.playerId, actionEvent.player);
		}
	}

private:
	//------------------------------------------------------------------------
	// Definition of all the actions. Action can be tailored to each player using playerID
	// ------------------------------------------------------------------------
	void PerformMoveUp(Input::PlayerID playerId, const Entity player)
	{
		const auto& inputComponent = player.GetComponent<InputComponent>();
		auto& rigidbody = player.GetComponent<RigidBodyComponent>();
		auto& animationComponent = player.GetComponent<AnimationComponent>();
		auto& spriteComponent = player.GetComponent<SpriteComponent>();

		rigidbody.velocity.y = inputComponent.upVelocity;
		animationComponent.animationFramesRow = Asset::DemoPlayer::ANIM_FORWARDS;
		animationComponent.bIsActive = true;
		spriteComponent.frame = Asset::DemoPlayer::ANIM_FORWARDS;
	}

	void PerformMoveRight(Input::PlayerID playerId, const Entity player)
	{
		const auto& inputComponent = player.GetComponent<InputComponent>();
		auto& rigidbody = player.GetComponent<RigidBodyComponent>();
		auto& animationComponent = player.GetComponent<AnimationComponent>();
		auto& spriteComponent = player.GetComponent<SpriteComponent>();

		rigidbody.velocity.x = inputComponent.rightVelocity;
		animationComponent.animationFramesRow = Asset::DemoPlayer::ANIM_RIGHT;
		animationComponent.bIsActive = true;
		spriteComponent.frame = Asset::DemoPlayer::ANIM_RIGHT;
	}

	void PerformMoveDown(Input::PlayerID playerId, const Entity player)
	{
		const auto& inputComponent = player.GetComponent<InputComponent>();
		auto& rigidbody = player.GetComponent<RigidBodyComponent>();
		auto& animationComponent = player.GetComponent<AnimationComponent>();
		auto& spriteComponent = player.GetComponent<SpriteComponent>();

		rigidbody.velocity.y = -inputComponent.downVelocity;
		animationComponent.animationFramesRow = Asset::DemoPlayer::ANIM_BACKWARDS;
		animationComponent.bIsActive = true;
		spriteComponent.frame = Asset::DemoPlayer::ANIM_BACKWARDS;
	}

	void PerformMoveLeft(Input::PlayerID playerId, const Entity player)
	{
		const auto& inputComponent = player.GetComponent<InputComponent>();
		auto& rigidbody = player.GetComponent<RigidBodyComponent>();
		auto& animationComponent = player.GetComponent<AnimationComponent>();
		auto& spriteComponent = player.GetComponent<SpriteComponent>();

		rigidbody.velocity.x = -inputComponent.leftVelocity;
		animationComponent.animationFramesRow = Asset::DemoPlayer::ANIM_LEFT;
		animationComponent.bIsActive = true;
		spriteComponent.frame = Asset::DemoPlayer::ANIM_LEFT;
	}

	void PerformJump(Input::PlayerID playerId, const Entity player)
	{
		const auto& inputComponent = player.GetComponent<InputComponent>();
		auto& rigidbody = player.GetComponent<RigidBodyComponent>();
		// auto& animationComponent = player.GetComponent<AnimationComponent>();
		// auto& spriteComponent = player.GetComponent<SpriteComponent>();

		rigidbody.velocity.y = inputComponent.upVelocity * 2;
		// animationComponent.animationFramesRow = Asset::DemoPlayer::ANIM_FORWARDS;
	}
	void PerformIdle(Input::PlayerID playerId, const Entity player)
	{
		// const auto& inputComponent = player.GetComponent<InputComponent>();
		auto& rigidbody = player.GetComponent<RigidBodyComponent>();
		auto& animationComponent = player.GetComponent<AnimationComponent>();

		rigidbody.velocity = Vector2();
		// animationComponent.animationFramesRow = Asset::DemoPlayer::ANIM_FORWARDS;
		animationComponent.bIsActive = false;
	}
};