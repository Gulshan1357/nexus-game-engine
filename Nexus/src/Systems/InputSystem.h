#pragma once

#include <iostream>
#include <memory>

#include "src/Components/AnimationComponent.h"
#include "src/ECS/System.h"
#include "src/ECS/Entity.h"
#include "src/EventManagement/EventManager.h"

#include "src/Events/ActionChangeEvent.h"
#include "src/InputManagement/InputEnums.h"

#include "src/Components/PlayerComponent.h"
#include "src/Components/RigidBodyComponent.h"

#include "src/Physics/Constants.h"

//------------------------------------------------------------------------
// The input system listens to Input events (Keyboard key press) and perform necessary action.
// ------------------------------------------------------------------------
class InputSystem : public System
{
public:
	InputSystem()
	{
		RequireComponent<PlayerComponent>();
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
		if (actionEvent.action == Input::PlayerAction::LMOUSE )
		{
			if (m_isDragging == false)
			{
				m_isDragging = true;
				App::GetMousePos(clickAndDrag.startMousePos.x, clickAndDrag.startMousePos.y);
			} else
			{
				m_isDragging = false;
				App::GetMousePos(clickAndDrag.endMousePos.x, clickAndDrag.endMousePos.y);
				LaunchBall(actionEvent.player, clickAndDrag);
			}
		}
	}

private:

	bool m_isDragging = false;
	struct ClickAndDrag
	{
		Vector2 startMousePos;
		Vector2 endMousePos;
	};
	ClickAndDrag clickAndDrag;
	
	//------------------------------------------------------------------------
	// Definition of all the actions. Action can be tailored to each player using playerID
	// ------------------------------------------------------------------------
	void LaunchBall(const Entity& player, ClickAndDrag clickAndDrag)
	{
		Logger::Log(clickAndDrag.startMousePos.ToString());
		Logger::Log(clickAndDrag.endMousePos.ToString());
		if (player.HasTag("Player1"))
		{
			// Calculate force and direction
			Vector2 direction = clickAndDrag.endMousePos - clickAndDrag.startMousePos;
        
			// Keep direction normalized but maintain the original magnitude for force
			float magnitude = direction.MagnitudeSquared();
			direction = -direction.Normalize();
        
			// Apply force directly proportional to drag distance
			Vector2 force = direction * magnitude;
        
			// Apply the force to the rigidbody
			player.GetComponent<RigidBodyComponent>().AddForce(force);
		}
	}
};