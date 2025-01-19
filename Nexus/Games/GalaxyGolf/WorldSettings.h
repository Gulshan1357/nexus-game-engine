#pragma once

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

	WorldSettings() : gravity(-9.8f), windSpeed(0.f), atmosphereDrag(0.01f) {}

	WorldSettings(const float gravity, const float windSpeed, const float atmosphereDrag, const float surfaceFriction, const float surfaceBounciness)
		: gravity(gravity),
		windSpeed(windSpeed),
		atmosphereDrag(atmosphereDrag)
	{}
};