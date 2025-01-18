#pragma once

#include <memory>

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"
#include "src/EventManagement/EventManager.h"

#include "src/Events/ActionChangeEvent.h"
#include "src/InputManagement/InputEnums.h"

#include "src/Components/PlayerComponent.h"
#include "src/Components/RigidBodyComponent.h"


//------------------------------------------------------------------------
// The input system listens to Input events (Keyboard key press) and perform necessary action.
// ------------------------------------------------------------------------
class InputSystem : public System
{
public:
	InputSystem()
	{
		RequireComponent<PlayerComponent>();
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
		if (actionEvent.action == Input::PlayerAction::LMOUSE)
		{
			auto& playerComponent = actionEvent.player.GetComponent<PlayerComponent>();
			if (playerComponent.bIsMouseClickHold == false)
			{
				playerComponent.bIsMouseClickHold = true;
				App::GetMousePos(m_startMousePos.x, m_startMousePos.y);
			}
			else
			{
				playerComponent.bIsMouseClickHold = false;
				App::GetMousePos(m_endMousePos.x, m_endMousePos.y);
				LaunchBall(actionEvent.player, m_startMousePos, m_endMousePos);
			}
		}
	}

private:
	Vector2 m_startMousePos;
	Vector2 m_endMousePos;

	//------------------------------------------------------------------------
	// Definition of all the actions. Action can be tailored to each player using playerID
	// ------------------------------------------------------------------------
	void LaunchBall(const Entity& player, const Vector2& startMousePos, const Vector2& endMousePos)
	{
		// Logger::Log(clickAndDrag.startMousePos.ToString());
		// Logger::Log(clickAndDrag.endMousePos.ToString());
		if (player.HasTag("Player1"))
		{
			// Calculate force and direction
			Vector2 direction = endMousePos - startMousePos;

			// Keep direction normalized but maintain the original magnitude for force
			float magnitude = direction.MagnitudeSquared();
			direction = -direction.Normalize();

			// Apply force directly proportional to drag distance
			Vector2 force = direction * magnitude;

			// Apply the force to the rigidbody
			player.GetComponent<RigidBodyComponent>().AddForce(force);

			player.GetComponent<PlayerComponent>().totalStrokes += 1;
		}
	}
};