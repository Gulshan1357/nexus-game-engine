#pragma once
#include "src/Utils/Color.h"

enum class WorldType
{
	EARTH,
	MARS,
	SUPER_EARTH,
};

struct WorldSettings
{
	float gravity;
	float windSpeed;
	float atmosphereDrag;
	Color groundColor;

	WorldSettings() : gravity(-9.8f), windSpeed(0.f), atmosphereDrag(0.01f), groundColor(Color(Colors::GRAY)) {}

	WorldSettings(
		const float gravity,
		const float windSpeed,
		const float atmosphereDrag,
		const float surfaceFriction,
		const float surfaceBounciness,
		const Color groundColor)
		: gravity(gravity),
		windSpeed(windSpeed),
		atmosphereDrag(atmosphereDrag),
		groundColor(groundColor)
	{}
};