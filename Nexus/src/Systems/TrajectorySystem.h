#pragma once


#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/Components/PlayerComponent.h"
#include "src/Components/RigidBodyComponent.h"
#include "src/Components/TransformComponent.h"
#include "src/Utils/GraphicsUtils.h"
#include "src/Physics/Camera.h"

class TrajectorySystem : public System
{
private:
	float m_gravity = 9.8f * 600;
	float m_timeStep = 0.1f;
	int m_maxSteps = 10;

public:
	TrajectorySystem()
	{
		RequireComponent<PlayerComponent>();
	}

	// deltaTime is in seconds
	void Update(const float deltaTime)
	{
		m_timeStep = deltaTime;
		for (auto& entity : GetSystemEntities())
		{
			auto& player = entity.GetComponent<PlayerComponent>();

			// Capture start position when mouse is first held
			if (player.bIsMouseClickHold && !m_wasMouseHeld)
			{
				App::GetMousePos(m_startMousePos.x, m_startMousePos.y);
				m_wasMouseHeld = true;
			}
			else if (!player.bIsMouseClickHold && m_wasMouseHeld)
			{
				m_wasMouseHeld = false;
			}
		}
	}

	void Render(const Camera& camera)
	{
		for (auto& entity : GetSystemEntities())
		{
			const auto& player = entity.GetComponent<PlayerComponent>();
			if (player.bIsMouseClickHold)
			{
				Vector2 currentMousePos;
				App::GetMousePos(currentMousePos.x, currentMousePos.y);

				const auto& transform = entity.GetComponent<TransformComponent>();
				CalculateTrajectory(transform.position, m_startMousePos, currentMousePos);

				for (size_t i = 0; i < trajectoryPoints.size() - 1; ++i)
				{
					Graphics::DrawCircle(
						Camera::WorldToScreen(trajectoryPoints[i], camera),
						2.f,
						6,
						{ 1.0f, 1.0f, 1.0f }
					);

					Graphics::DrawLine(
						Camera::WorldToScreen(trajectoryPoints[i], camera),
						Camera::WorldToScreen(trajectoryPoints[i + 1], camera),
						{ 1.0f, 1.0f, 1.0f }
					);
				}
			}
		}
	}


private:
	std::vector<Vector2> trajectoryPoints;

	Vector2 m_startMousePos;
	bool m_wasMouseHeld = false;

	void CalculateTrajectory(const Vector2& playerPos, const Vector2& startMouse, const Vector2& currentMouse)
	{
		trajectoryPoints.clear();

		// Calculate force direction and magnitude
		Vector2 direction = currentMouse - startMouse;
		const float magnitude = direction.MagnitudeSquared();
		direction = -direction.Normalize();  // Match your input system

		// Scale the initial velocity
		const Vector2 initialVelocity = direction * (magnitude * 0.01f);

		Vector2 position = playerPos;
		Vector2 velocity = initialVelocity;

		trajectoryPoints.push_back(position);

		for (int i = 1; i < m_maxSteps; ++i)
		{
			velocity.y -= m_gravity * m_timeStep;
			position += velocity * m_timeStep;
			trajectoryPoints.push_back(position);
		}
	}
};
