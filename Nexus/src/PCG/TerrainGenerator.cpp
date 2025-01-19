#include "stdafx.h"

#include "TerrainGenerator.h"

#include "src/Physics/Camera.h"
#include "src/Utils/GraphicsUtils.h"
#include "src/Utils/Random.h"


inline std::vector<Vector2> TerrainGenerator::GenerateTerrain(float startX, const float startY, const Config& customConfig)
{
	Config currentConfig = customConfig;

	float currentX = startX;
	float currentY = startY;

	std::vector<Vector2> upperPoints;
	upperPoints.reserve(currentConfig.numSegments);

	// Generate segments
	for (int i = 0; i < currentConfig.numSegments - 1; ++i)
	{
		const float segmentLength = Random::Float(currentConfig.minSegmentLength, currentConfig.maxSegmentLength);
		const float rawHeightDiff = Random::Float(currentConfig.minHeightDiff, currentConfig.maxHeightDiff);

		// Apply smoothing
		const float smoothedHeightDiff = rawHeightDiff * (1 - currentConfig.smoothingFactor) +
			m_prevHeightDiff * currentConfig.smoothingFactor;

		currentX += segmentLength;
		currentY += smoothedHeightDiff;
		currentY = std::clamp(currentY, currentConfig.minHeight, currentConfig.maxHeight);

		upperPoints.emplace_back(currentX, currentY);
		m_prevHeightDiff = smoothedHeightDiff;
	}
	return upperPoints;
}

std::vector<Vector2> TerrainGenerator::GenerateEasyTerrain(const float startX, const float startY)
{
	Config easyConfig;
	easyConfig.minHeightDiff = -40.0f;
	easyConfig.maxHeightDiff = 40.0f;
	easyConfig.smoothingFactor = 0.7f;
	easyConfig.numSegments = 10;
	return GenerateTerrain(startX, startY, easyConfig);
}

std::vector<Vector2> TerrainGenerator::GenerateHardTerrain(const float startX, const float startY, const float width)
{
	Config hardConfig;
	hardConfig.minHeightDiff = -100.0f;
	hardConfig.maxHeightDiff = 100.0f;
	hardConfig.smoothingFactor = 0.3f;
	hardConfig.numSegments = 15;
	return GenerateTerrain(startX, startY, hardConfig);
}


