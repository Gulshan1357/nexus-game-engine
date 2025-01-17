#pragma once

#include "Color.h"
#include "Vector2.h"
#include <algorithm>

namespace Math
{
	// Linear interpolation for floating point values
	[[nodiscard]] inline float Lerp(const float start, const float end, float t)
	{
		t = std::clamp(t, 0.0f, 1.0f);
		return start + (end - start) * t;
	}

	// Linear interpolation for Vector2
	[[nodiscard]] inline Vector2 Lerp(const Vector2& start, const Vector2& end, float t)
	{
		t = std::clamp(t, 0.0f, 1.0f);
		return {
			start.x + (end.x - start.x) * t,
			start.y + (end.y - start.y) * t
		};
	}

	// Linear interpolation for Color
	[[nodiscard]] inline Color Lerp(const Color& start, const Color& end, float t)
	{
		t = std::clamp(t, 0.0f, 1.0f);

		return {
			start.r + (end.r - start.r) * t,
			start.g + (end.g - start.g) * t,
			start.b + (end.b - start.b) * t
		};
	}
}