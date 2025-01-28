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

	// Responsible for generating terrain, colliders, win hole, and obstacles
	static std::vector<Vector2> GenerateLevel(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, const PCGConfig pcgConfig);

	// Render the Terrain with fading color(to black) based on the terrain points
	static void RenderTerrain(const Camera& camera, const std::vector<Vector2>& terrainVertices, Color color);

	// Spawn win hole near the end. It also modifies the terrain vertices to make sure the hole is on the level ground
	static void SpawnHole(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, Vector2 position);

	// Spawn polygon colliders based on the terrain vertices.
	static void AddTerrain(const std::unique_ptr<Coordinator>& coordinator, const std::vector<Vector2>& terrainVertices, const float elasticity, const float friction);
	// Spawn random obstacles based on the terrain vertices
	static void AddObstacles(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, std::vector<Vector2>& terrainVertices);

	// Spawn laser, it kills the ball on touch
	static void SpawnLaser(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, const Vector2 firstPoint, const Vector2 secondPoint);
	// Spawn pendulum using spring force
	static void SpawnPendulum(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, Vector2 terrainPoint);
	// Spawn connected beads. First two beads are connected using spring force and the rest using joint constraints
	static void SpawnConnectedBeads(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, Vector2 terrainPoint);

	// Destructible Obstacle
	enum class ShapeType
	{
		// CIRCLE,
		// TRIANGLE,
		SQUARE,
		RECTANGLE
	};
	// Spawn random shapes (rectangle and square)
	static void SpawnShapes(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, Vector2& tTerrainPoint);

	static void SpawnExplosiveShape(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, Vector2 spawnPosition, ShapeType shapeType);
	static void SpawnGlassShape(const std::unique_ptr<Coordinator>& coordinator,
		const std::unique_ptr<AssetManager>& assetManager, Vector2 spawnPosition,
		ShapeType shapeType);
	static void SpawnWoodenShape(const std::unique_ptr<Coordinator>& coordinator,
		const std::unique_ptr<AssetManager>& assetManager, Vector2 spawnPosition, ShapeType shapeType);
	static void SpawnStoneShape(const std::unique_ptr<Coordinator>& coordinator,
		const std::unique_ptr<AssetManager>& assetManager, Vector2 spawnPosition, ShapeType shapeType);

};
