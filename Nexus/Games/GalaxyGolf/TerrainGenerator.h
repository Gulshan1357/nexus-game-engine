#pragma once
#include <random>

#include "src/Utils/Color.h"
#include "src/Utils/GraphicsUtils.h"
#include "src/Utils/Random.h"
#include "src/Utils/Vector2.h"

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
private:
	// Configuration
	Config m_config;
	float m_prevHeightDiff = 0.0f;

public:
	TerrainGenerator() = default;

	std::vector<Vector2> GenerateTerrain(float startX, const float startY, const Config& customConfig = Config())
	{
		Config currentConfig = customConfig;

		float currentX = startX;
		float currentY = startY;

		std::vector<Vector2> upperPoints;
		upperPoints.reserve(currentConfig.numSegments + 2); // +2 start and end vertex. They are elevated to act as boundary
		upperPoints.emplace_back(startX - 300.f, startY + 600.f);

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

		upperPoints.emplace_back(upperPoints.back().x + 300.f, startY + 600.f);
		return upperPoints;
	}

	// Generate terrain with different difficulty levels
	std::vector<Vector2> GenerateEasyTerrain(const float startX, const float startY)
	{
		Config easyConfig;
		easyConfig.minHeightDiff = -40.0f;
		easyConfig.maxHeightDiff = 40.0f;
		easyConfig.smoothingFactor = 0.7f;
		easyConfig.numSegments = 10;
		return GenerateTerrain(startX, startY, easyConfig);
	}

	std::vector<Vector2> GenerateHardTerrain(const float startX, const float startY, const float width)
	{
		Config hardConfig;
		hardConfig.minHeightDiff = -100.0f;
		hardConfig.maxHeightDiff = 100.0f;
		hardConfig.smoothingFactor = 0.3f;
		hardConfig.numSegments = 15;
		return GenerateTerrain(startX, startY, hardConfig);
	}

	// Render the terrain
	static void Render(const Camera& camera, const std::vector<Vector2>& terrainVertices, const Color color)
	{
		// Check if the terrain vertices are empty or invalid
		if (terrainVertices.empty())
		{
			return;
		}

		// The terrainVertices are from left to right, but we need a polygon ordered anti-clockwise starting from bottom left
		std::vector<Vector2> polygonVertices;
		polygonVertices.reserve(terrainVertices.size() + 2); // +2 start and end vertex.

		polygonVertices.emplace_back(terrainVertices.front().x, -600.f); // bottom left
		polygonVertices.emplace_back(terrainVertices.back().x, -600.f); // bottom right
		for (auto it = terrainVertices.rbegin(); it != terrainVertices.rend(); ++it)
		{
			polygonVertices.push_back(*it);
		}

		// Convert world-space terrain vertices to screen-space
		std::vector<Vector2> screenVertices;
		screenVertices.reserve(polygonVertices.size());
		for (const auto& vertex : polygonVertices)
		{
			screenVertices.push_back(Camera::WorldToScreen(vertex, camera));
			Graphics::DrawCircle(Camera::WorldToScreen(vertex, camera), 2.f, 4, Color(Colors::RED));
		}

		// Draw the filled polygon representing the terrain
		Graphics::DrawFillPolygon(screenVertices, color);
	}
};
