#pragma once
#include <random>

#include "src/Utils/Vector2.h"

class Camera;

class TerrainGenerator
{
public:
	struct Config
	{
		float minSegmentLength = 50.0f;
		float maxSegmentLength = 150.0f;
		float minHeightDiff = -30.0f;
		float maxHeightDiff = 30.0f;
		float smoothingFactor = 0.5f;
		float minHeight = 100.0f;
		float maxHeight = 700.0f;
		int numSegments = 10;
	};

	TerrainGenerator() = default;

	std::vector<Vector2> GenerateTerrain(float startX, const float startY, const Config& customConfig = Config());

	// Generate terrain with different difficulty levels
	std::vector<Vector2> GenerateEasyTerrain(const float startX, const float startY);

	std::vector<Vector2> GenerateHardTerrain(const float startX, const float startY, const float width);

private:
	Config m_config;
	float m_prevHeightDiff = 0.0f;
};
