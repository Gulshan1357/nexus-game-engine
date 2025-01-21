#pragma once

#include <chrono>

#include "src/ECS/Entity.h"
#include "src/ECS/System.h"

#include "src/Components/PlayerComponent.h"

// #include "src/Utils/Font.h"
#include "src/Utils/GraphicsUtils.h"


class RenderHUDSystem : public System
{
public:
	RenderHUDSystem()
	{
		RequireComponent<PlayerComponent>();
		startTime = std::chrono::steady_clock::now();
	}

	void Update(const Camera& camera, const WorldType& worldType, const WorldSettings& worldSettings, Color color) const
	{
		// Top Left (Time and World Setting)
		DisplayTimeElapsed(color, { 10, Physics::SCREEN_HEIGHT - 20 });
		DisplayWorldSetting(color, { 10, Physics::SCREEN_HEIGHT - 50 }, worldType, worldSettings);

		// Top Right (Player scores)
		Vector2 scorePos = { Physics::SCREEN_WIDTH - 150, Physics::SCREEN_HEIGHT };
		for (auto& entity : GetSystemEntities())
		{
			const auto& player = entity.GetComponent<PlayerComponent>();

			std::string scoreText;
			if (entity.HasTag("Player1")) scoreText = "Player One strokes: " + std::to_string(player.totalStrokes);
			else if (entity.HasTag("Player2")) scoreText = "Player Two strokes: " + std::to_string(player.totalStrokes);
			scorePos.y -= 20.f;
			Graphics::PrintText(scoreText, scorePos, color);

			if (entity.HasTag("Player1")) DisplayControls(color, {Physics::SCREEN_WIDTH/2.f - 100.f, Physics::SCREEN_HEIGHT - 40.f}, player.activeAbility);
		}
	}

private:
	std::chrono::steady_clock::time_point startTime;

	void DisplayTimeElapsed(const Color color, const Vector2 timePos) const
	{

		// Calculate elapsed time
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>
			(currentTime - startTime).count();

		// Format time as MM:SS
		int minutes = static_cast<int>(elapsedSeconds) / 60;
		int seconds = static_cast<int>(elapsedSeconds) % 60;
		std::string timeText = std::string("Time: ") +
			(minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
			(seconds < 10 ? "0" : "") + std::to_string(seconds);

		Graphics::PrintText(timeText, timePos, color);
	}

	void DisplayControls(const Color color, const Vector2 pos, const Ability activeAbility) const
	{
		switch (activeAbility) {
		case Ability::NORMAL_SHOT:
			Graphics::PrintText("Active Ability: Normal shot", pos, color);
			break;
		case Ability::POWER_SHOT:
			Graphics::PrintText("Active Ability: Power shot (Twice as powerful)", pos, color);
			break;
		case Ability::WEAK_SHOT:
			Graphics::PrintText("Active Ability: Weak shot (Weak but precise in low gravity/friction/drag worlds)", pos, color);
			break;
		}
		
	}

	static void DisplayWorldSetting(const Color color, Vector2 position, const WorldType& worldType, const WorldSettings& worldSettings)
	{
		// Display world name
		std::string worldNameText;
		switch (worldType)
		{
			case WorldType::EARTH:
				worldNameText = "World: Earth";
				break;
			case WorldType::MARS:
				worldNameText = "World: Mars";
				break;
			case WorldType::SUPER_EARTH:
				worldNameText = "World: Super Earth";
				break;
		}
		Graphics::PrintText(worldNameText, position, color);

		position.y -= 20;

		// Display wind speed
		const std::string windSpeedText = "Wind Speed: " + std::to_string(worldSettings.windSpeed);
		Graphics::PrintText(windSpeedText, position, color);

		position.y -= 20;

		// Display gravity
		std::string gravityText = "Gravity: " + std::to_string(worldSettings.gravity) + " m/s^2";
		Graphics::PrintText(gravityText, position, color);

	}
};
