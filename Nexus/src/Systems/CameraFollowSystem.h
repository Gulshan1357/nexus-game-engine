#pragma once

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/CameraFollowComponent.h"
#include "src/Components/TransformComponent.h"

#include "src/Physics/Camera.h"
#include "src/Utils/Math.h"

//------------------------------------------------------------------------
// System that handles camera movement to follow one or two players.
// - Single player: Camera follows the player with a small deadzone
// - Two players: Camera keeps both players in view
// Only considers the first two entities with CameraFollowComponent & TransformComponent
// ------------------------------------------------------------------------
class CameraFollowSystem : public System
{
private:
	// Camera settings for single player mode
	float m_singlePlayerDeadzone = 15.0f;		// Distance before camera moves
	float m_singlePlayerSmoothing = 0.07f;		// Movement interpolation factor

	// Camera settings for two player mode
	float m_twoPlayerDeadzone = 25.0f;			// Distance before camera moves
	float m_twoPlayerSmoothing = 0.05f;			// Movement interpolation factor
	float m_minPlayerDistance = 150.0f;			// Minimum distance before camera moves (two player only)

	// General camera settings
	Vector2 m_cameraOffset = { 200.0f, 200.0f };	// Base offset from target position

public:
	CameraFollowSystem()
	{
		RequireComponent<CameraFollowComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(Camera& camera) const
	{
		auto& entities = GetSystemEntities();
		if (entities.empty()) return;

		const bool isSinglePlayer = entities.size() == 1;

		const float currentDeadzone = isSinglePlayer ? m_singlePlayerDeadzone : m_twoPlayerDeadzone;
		const float currentSmoothing = isSinglePlayer ? m_singlePlayerSmoothing : m_twoPlayerSmoothing;

		Vector2 targetPos;
		if (isSinglePlayer)
		{
			// Single player: Follow the player with offset
			const auto& transform = entities[0].GetComponent<TransformComponent>();
			targetPos = transform.position + (m_cameraOffset / 2.f);
		}
		else
		{
			// Two players: Position camera to keep both in view.
			const auto& transform1 = entities[0].GetComponent<TransformComponent>();
			const auto& transform2 = entities[1].GetComponent<TransformComponent>();

			// Calculate distance between players
			const float distanceX = std::abs(transform1.position.x - transform2.position.x);
			const float distanceY = std::abs(transform1.position.y - transform2.position.y);

			// Only move camera if players are far enough apart
			if (distanceX > m_minPlayerDistance || distanceY > m_minPlayerDistance)
			{
				// Calculate midpoint between players (TODO: handle offset for 2 players)
				targetPos = Vector2(
					(transform1.position.x + transform2.position.x) * 0.5f,
					(transform1.position.y + transform2.position.y) * 0.5f
				);
				// targetPos = Vector2(
				// 	(transform1.position.x + transform2.position.x) * 0.5f,
				// 	(transform1.position.y + transform2.position.y) * 0.5f
				// ) + m_cameraOffset;
			}
			else
			{
				// Keep camera still if players are close together
				targetPos = camera.GetPosition();
			}
		}

		// Update camera position with deadzone and smoothing
		const Vector2 currentPos = camera.GetPosition();
		Vector2 newPos = currentPos;

		// Only move camera if target is outside deadzone
		if (std::abs(targetPos.x - currentPos.x) > currentDeadzone)
		{
			newPos.x = Math::Lerp(currentPos.x, targetPos.x, currentSmoothing);
		}
		if (std::abs(targetPos.y - currentPos.y) > currentDeadzone)
		{
			newPos.y = Math::Lerp(currentPos.y, targetPos.y, currentSmoothing);
		}

		camera.SetPosition(newPos);
	}
};
