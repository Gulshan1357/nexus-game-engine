#include "stdafx.h"

#include "PCG.h"

#include "src/AssetManagement/AssetManager.h"
#include "src/Components/AnimationComponent.h"
#include "src/ECS/Coordinator.h"
#include "src/ECS/Entity.h"

#include "src/Components/ColliderTypeComponent.h"
#include "src/Components/PolygonColliderComponent.h"
#include "src/Components/RigidBodyComponent.h"
#include "src/Components/SpriteComponent.h"
#include "src/Components/TransformComponent.h"
#include "src/Components/BoxColliderComponent.h"
#include "src/Components/CircleColliderComponent.h"

#include "src/Physics/Camera.h"
#include "src/Utils/Color.h"
#include "src/Utils/GraphicsUtils.h"
#include "src/Utils/Random.h"

std::vector<Vector2> PCG::GenerateLevel(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, const PCGConfig pcgConfig)
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

	// Add a couple of points at the end so that the hole(goal) is on perfectly horizontal level
	terrainVertices.emplace_back(terrainVertices.back().x + 50.f, terrainVertices.back().y);
	terrainVertices.emplace_back(terrainVertices.back().x + 50.f, terrainVertices.back().y);

	// Create a dip for Hole
	terrainVertices.emplace_back(terrainVertices.back().x + 20.f, terrainVertices.back().y - 25.f);
	SpawnHole(coordinator, assetManager, { terrainVertices.back().x + 10.f,terrainVertices.back().y + 25.f });
	terrainVertices.emplace_back(terrainVertices.back().x + 20.f, terrainVertices.back().y);

	// Return to level and add some more points
	terrainVertices.emplace_back(terrainVertices.back().x + 20.f, terrainVertices.back().y + 25.f);
	for (int i = 0; i < 5; i++) terrainVertices.emplace_back(terrainVertices.back().x + 50.f, terrainVertices.back().y);

	// Elevated last point
	terrainVertices.emplace_back(terrainVertices.back().x + 300.f, terrainVertices.back().y + 600.f);

	// Place colliders on ground surface
	AddTerrain(coordinator, terrainVertices, pcgConfig.elasticity, pcgConfig.friction);

	// From this point don't modify terrainVertices these are used to render ground color

	// TODO: obstacles
	AddObstacles(coordinator, assetManager, terrainVertices);

	return terrainVertices;
}

void PCG::SpawnHole(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, Vector2 position)
{
	assetManager->AddSprite("hole", R"(.\Assets\Sprites\hole.bmp)", 1, 1);
	assetManager->AddSprite("flag", R"(.\Assets\Sprites\Flags\FlagRed.bmp)", 7, 1);

	Entity hole = coordinator->CreateEntity();
	hole.AddComponent<SpriteComponent>("hole", 3);
	hole.AddComponent<TransformComponent>(Vector2(position.x, position.y - 15.f), Vector2(1.f, 1.f));
	hole.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 0.f, 0.f, 0.0f, 0.1f, 0.1f);
	hole.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	hole.AddComponent<BoxColliderComponent>(
		assetManager->GetSpriteWidth("hole") / 2.f,
		assetManager->GetSpriteWidth("hole") / 2.f,
		Vector2(0.f, -25.f) // offset
	);
	hole.Tag("Hole");


	Entity flag = coordinator->CreateEntity();
	flag.AddComponent<TransformComponent>(Vector2(position.x + 70.f, position.y + 30.f), Vector2(1.f, 1.f));
	flag.AddComponent<SpriteComponent>("flag", 3);
	flag.AddComponent<AnimationComponent>(true, 7, true);

	// Entity flag2 = m_coordinator->CreateEntity();
	// flag2.AddComponent<TransformComponent>(Vector2(250.f, 45.f), Vector2(1.f, 1.f));
	// flag2.AddComponent<SpriteComponent>("flag", 3);
}

void PCG::AddTerrain(const std::unique_ptr<Coordinator>& coordinator, const std::vector<Vector2>& terrainVertices, const float elasticity, const float friction)
{
	for (size_t i = 0; i < terrainVertices.size() - 1; i++)
	{
		const Vector2& currentPos = terrainVertices[i];
		const Vector2& nextPos = terrainVertices[i + 1];
		float xDifference = nextPos.x - currentPos.x;
		float yDifference = nextPos.y - currentPos.y;
		constexpr float colliderDepth = 500.f;

		// Terrain entity will be a point with polygon collider stretching till the next point
		Entity terrain = coordinator->CreateEntity();
		terrain.AddComponent<TransformComponent>(currentPos, Vector2(1.f, 1.f));

		// Add collider type
		terrain.AddComponent<ColliderTypeComponent>(ColliderType::Polygon);
		std::vector<Vector2> polygonVertices;
		polygonVertices.emplace_back(0.f, -colliderDepth); // Bottom left
		polygonVertices.emplace_back(xDifference, yDifference - colliderDepth);    // Bottom right
		polygonVertices.emplace_back(xDifference, yDifference);    // Top right
		polygonVertices.emplace_back(0.f, 0.f); // Top left
		terrain.AddComponent<PolygonColliderComponent>(polygonVertices);
		terrain.AddComponent<RigidBodyComponent>(Vector2(), Vector2(), false, 0.0f, 0.0f, 0.0f, elasticity, friction);
		terrain.Group("Terrain");

		// Adding a circle collider between 2 points because the collision resolution b/w circle and polygon vertex is not stable
		// Entity terrainConnector = m_coordinator->CreateEntity();
		// terrainConnector.AddComponent<TransformComponent>(nextPos, Vector2(1.f, 1.f));
		// terrainConnector.AddComponent<RigidBodyComponent>(Vector2(), Vector2(), false, 0.0f, 0.0f, 0.0f, elasticity, friction);
		// terrainConnector.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
		// terrainConnector.AddComponent<CircleColliderComponent>(1.f);
		// terrainConnector.Group("Terrain");
	}
}

void PCG::AddObstacles(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, const std::vector<Vector2>& terrainVertices)
{
	// Skip first and last vertices to avoid edges
	for (size_t i = 1; i < terrainVertices.size() - 1; i++)
	{
		// Random chance to spawn an obstacle (30% chance)
		if (Random::Float(0.0f, 1.0f) < 1.0f)
		{
			// Random obstacle type (1-3)
			int obstacleType = Random::Int(1, 3);

			// Spawn different obstacles based on random type
			switch (obstacleType)
			{
				case 1:
					SpawnLaser(coordinator, assetManager, terrainVertices[i], terrainVertices[i + 1]);
					break;
					// case 2:
					// 	// AddObstacle2(position);
					// 	break;
					// case 3:
					// 	// AddObstacle3(position);
					// 	break;
				default:;
			}

			i++;
		}
	}
}

void PCG::SpawnLaser(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager,
	const Vector2 firstPoint, const Vector2 secondPoint)
{
	float angle = std::atan2(secondPoint.y - firstPoint.y, secondPoint.x - firstPoint.x);

	// Add sprites for the laser and laser shooter
	assetManager->AddSprite("laser", R"(.\Assets\Sprites\Obstacles\laser.bmp)", 1, 1);
	assetManager->AddSprite("laser_shooter", R"(.\Assets\Sprites\Obstacles\laser_shooter.bmp)", 1, 1);

	// Create the first laser shooter entity (bottom)
	Entity shooter1 = coordinator->CreateEntity();
	shooter1.AddComponent<SpriteComponent>("laser_shooter", 3);
	shooter1.AddComponent<TransformComponent>(
		firstPoint,
		Vector2(1.f, 1.f), // Scale
		angle // Rotation
	);
	shooter1.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	shooter1.AddComponent<CircleColliderComponent>(assetManager->GetSpriteHeight("laser_shooter") * 0.42f);
	shooter1.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 0.f, 0.f, 0.0f, 0.1f, 0.1f);
	shooter1.Group("LaserShooter");

	// Create the laser entity
	Entity laser = coordinator->CreateEntity();
	float laserHeight = assetManager->GetSpriteHeight("laser");

	Vector2 laserPosition = Vector2(
		firstPoint.x - std::sin(angle) * (laserHeight / 1.7f),
		firstPoint.y + std::cos(angle) * (laserHeight / 1.7f)
	);

	laser.AddComponent<SpriteComponent>("laser", 3);
	laser.AddComponent<TransformComponent>(
		laserPosition,
		Vector2(1.f, 1.f),
		angle
	);
	laser.AddComponent<RigidBodyComponent>(
		Vector2(0.0f, 0.0f), Vector2(), false, 0.f, 0.f, 0.0f, 0.1f, 0.1f
	);
	laser.AddComponent<ColliderTypeComponent>(ColliderType::Box);
	laser.AddComponent<BoxColliderComponent>(assetManager->GetSpriteWidth("laser"), assetManager->GetSpriteHeight("laser") * 0.9f);
	laser.Group("StaticKillers");
}

void PCG::RenderTerrain(const Camera& camera, const std::vector<Vector2>& terrainVertices, const Color color)
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
