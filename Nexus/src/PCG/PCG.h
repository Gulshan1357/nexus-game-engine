#pragma once

#include "TerrainGenerator.h"

class AssetManager;
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

	static std::vector<Vector2> GenerateLevel(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, const PCGConfig pcgConfig);

	static void SpawnHole(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, Vector2 position);

	static void AddTerrain(const std::unique_ptr<Coordinator>& coordinator, const std::vector<Vector2>& terrainVertices, const float elasticity, const float friction);
	static void RenderTerrain(const Camera& camera, const std::vector<Vector2>& terrainVertices, Color color);
};
