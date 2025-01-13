#include "stdafx.h"
#include "UIEffects.h"

#include <random>

#include "src/Utils/GraphicsUtils.h"
#include <src/Physics/Constants.h>
#include "src/Utils/Vector2.h"
#include "src/Utils/Color.h"

void UIEffects::RenderStartField(const Color& color, const int startCount, const int randomSeed)
{
	std::mt19937 rng(randomSeed);
	std::uniform_real_distribution<float> distX(0.0f, Physics::SCREEN_WIDTH);
	std::uniform_real_distribution<float> distY(0.0f, Physics::SCREEN_HEIGHT);

	constexpr float starRadius = 1.0f;
	constexpr int starSegments = 8;

	for (int i = 0; i < startCount; ++i)
	{
		Vector2 position{ distX(rng), distY(rng) };
		Graphics::DrawCircle(position, starRadius, starSegments, color);
	}
}

void UIEffects::DrawConcentricCircles(const Vector2& center, const int numCircles, const float initialRadius, const float radiusIncrement, const int segments, const Color& color)
{
	// Draw each circle with increasing radius
	for (int i = 0; i < numCircles; i++)
	{
		const float radius = initialRadius + (static_cast<float>(i) * radiusIncrement);
		Graphics::DrawCircle(center, radius, segments, color);
	}
}

void UIEffects::RenderFadingBackground(const Color& topColor, float fadeStrength)
{
	constexpr int numStrips = 250;
	constexpr float stripHeight = Physics::SCREEN_HEIGHT / static_cast<float>(numStrips);

	for (int i = 0; i < numStrips; ++i)
	{
		const float fadeFactor = std::pow(static_cast<float>(i) / numStrips, fadeStrength);
		const Color stripColor{
			topColor.r * fadeFactor,
			topColor.g * fadeFactor,
			topColor.b * fadeFactor
		};

		Graphics::DrawFillRectangle(
			Vector2{ 0.0f, static_cast<float>(i) * stripHeight },
			Physics::SCREEN_WIDTH,
			stripHeight + 1.0f, // Strip overlap = 1.0f
			stripColor
		);
	}
}

