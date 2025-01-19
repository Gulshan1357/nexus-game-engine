#pragma once

#include "TerrainGenerator.h"

struct Color;
class Coordinator;

class PCG
{
public:

	struct PCGConfig
	{
		float heightVariance = 700.f;
		int sizeOfLevel = 10;
		float elasticity;
		float friction;
	};

	static std::vector<Vector2> GenerateLevel(const std::unique_ptr<Coordinator>& coordinator, const PCGConfig pcgConfig)
	{
		// First generate a random terrain
		const TerrainGenerator::Config terrainConfig =
		{
			50.0f,
			150.0f,
			-100.0f,
			100.0f,
			0.5f,
			100.0f,
			pcgConfig.heightVariance,
			pcgConfig.sizeOfLevel,
		};
		TerrainGenerator generator;

		std::vector<Vector2> terrainVertices;

		// Elevated first point
		terrainVertices.emplace_back(-600.f, 600.f);
		// PCG points
		auto pcgPoints = generator.GenerateTerrain(-300.f, 0.f, terrainConfig);
		terrainVertices.insert(terrainVertices.end(), pcgPoints.begin(), pcgPoints.end());
		// Add one more point in the end so that the hole(goal) is on perfectly horizontal level
		terrainVertices.emplace_back(terrainVertices.back().x + 50.f, terrainVertices.back().y);
		// Elevated last point
		terrainVertices.emplace_back(terrainVertices.back().x + 300.f, terrainVertices.back().y + 600.f);

		// Place colliders on ground surface
		AddTerrain(coordinator, terrainVertices, pcgConfig.elasticity, pcgConfig.friction);

		// From this point don't modify terrainVertices these are used to render ground color

		// TODO: obstacles

		return terrainVertices;
	}

	static void AddTerrain(const std::unique_ptr<Coordinator>& coordinator, const std::vector<Vector2>& terrainVertices, const float elasticity, const float friction);
	static void RenderTerrain(const Camera& camera, const std::vector<Vector2>& terrainVertices, Color color);
};
