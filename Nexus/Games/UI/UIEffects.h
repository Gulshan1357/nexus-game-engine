#pragma once

#include "src/Utils/Color.h"

struct Vector2;

namespace UIEffects
{
	// Create star-field effect
	void RenderStartField(const Color& color = Color(Colors::RED), int startCount = 100, int randomSeed = 12345);

	// Function to draw concentric circles
	void DrawConcentricCircles(const Vector2& center, int numCircles, float initialRadius, float radiusIncrement, int segments, const Color& color);

	// Function to render a top to bottom color fading effect
	void RenderFadingBackground(const Color& topColor = Color(Colors::RED), float fadeStrength = 2.5f);
}
