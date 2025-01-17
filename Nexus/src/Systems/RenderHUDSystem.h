#pragma once

#include <chrono>

#include "src/ECS/Entity.h"
#include "src/ECS/System.h"

#include "src/Components/PlayerComponent.h"

#include "src/Utils/Font.h"
#include "src/Utils/GraphicsUtils.h"


class RenderHUDSystem : public System
{
public:
	RenderHUDSystem()
	{
		RequireComponent<PlayerComponent>();
		startTime = std::chrono::steady_clock::now();
	}

	void Update(const Camera& camera) const
	{
		for (auto entity : GetSystemEntities())
		{
			const auto& player = entity.GetComponent<PlayerComponent>();

			// Calculate elapsed time
			auto currentTime = std::chrono::steady_clock::now();
			auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>
								(currentTime - startTime).count();
			
			// Format time as MM:SS
			int minutes = static_cast<int>(elapsedSeconds) / 60;
			int seconds = static_cast<int>(elapsedSeconds) % 60;

			// Create score and time strings
			std::string scoreText = "Strokes: " + std::to_string(player.totalStrokes);
			std::string timeText = std::string("Time: ") +
					   (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
					   (seconds < 10 ? "0" : "") + std::to_string(seconds);

			// Position HUD elements
			// Score in top left
			Vector2 scorePos = {30, Physics::SCREEN_HEIGHT - 30}; // Adjust these values as needed
            
			// Time in top right
			Vector2 timePos = {Physics::SCREEN_WIDTH - 80, Physics::SCREEN_HEIGHT - 30};

			// Render the HUD elements
			Graphics::PrintText(scoreText, scorePos, Color(Colors::WHITE)); // White color
			Graphics::PrintText(timeText, timePos, Color(Colors::WHITE));
		}
	}

private:
	std::chrono::steady_clock::time_point startTime;
};
