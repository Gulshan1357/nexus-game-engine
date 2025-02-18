#pragma once

#include <memory>

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"
#include "src/EventManagement/EventManager.h"

#include "src/Events/ActionChangeEvent.h"
#include "src/InputManagement/InputEnums.h"

#include "src/Components/PlayerComponent.h"
#include "src/Components/RigidBodyComponent.h"
#include "src/Events/LaunchBallEvent.h"


//------------------------------------------------------------------------
// The input system listens to Input events (Keyboard key press) and perform emit necessary events.
// ------------------------------------------------------------------------
class InputSystem : public System
{
private:
	std::shared_ptr<EventManager> m_eventManager;
public:
	InputSystem()
	{
		RequireComponent<PlayerComponent>();
	}

	void SubscribeToEvents(const std::shared_ptr<EventManager>& eventManager)
	{
		// Create a new shared pointer from the reference. Need it to emit launch ball event events.
		m_eventManager = eventManager;

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
				EmitLaunchBallEvent(actionEvent.player, actionEvent.playerId, m_startMousePos, m_endMousePos);
			}
		}
	}

	void RenderForce(const Camera& camera)
	{
		Graphics::PrintText( "Last Shot Strength:" + m_force.ToString(),
			Vector2(Physics::SCREEN_WIDTH/2.f - 100.f, Physics::SCREEN_HEIGHT -20.f)
			);
	}

private:
	Vector2 m_startMousePos;
	Vector2 m_endMousePos;
	Vector2 m_force;

	//------------------------------------------------------------------------
	// Definition of all the actions. Action can be tailored to each player using playerID
	// ------------------------------------------------------------------------
	void EmitLaunchBallEvent(const Entity& player, Input::PlayerID playerId, const Vector2& startMousePos, const Vector2& endMousePos)
	{

		// Calculate force and direction
		Vector2 direction = endMousePos - startMousePos;

		float magnitude = direction.MagnitudeSquared();
		direction = -direction.Normalize();

		m_force = direction * magnitude;

		// Apply the force to the RigidBody
		m_eventManager->EmitEvent<LaunchBallEvent>(player, playerId, m_force);

	}
};