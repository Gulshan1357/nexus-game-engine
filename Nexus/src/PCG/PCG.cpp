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
#include "src/Components/ConstraintTypeComponent.h"
#include "src/Components/JointConstraintComponent.h"

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
	terrainVertices.emplace_back(-1000.f, 600.f);

	// Initial leveled ground
	terrainVertices.emplace_back(-200.f, 0.f);
	terrainVertices.emplace_back(-50.f, 0.f);

	// PCG points
	auto pcgPoints = generator.GenerateTerrain(0.f, 0.f, terrainConfig);
	terrainVertices.insert(terrainVertices.end(), pcgPoints.begin(), pcgPoints.end());

	// Add a couple of points at the end so that the hole(goal) is on perfectly horizontal level
	terrainVertices.emplace_back(terrainVertices.back().x + 50.f, terrainVertices.back().y);
	terrainVertices.emplace_back(terrainVertices.back().x + 300.f, terrainVertices.back().y);

	// Create a dip for Hole
	terrainVertices.emplace_back(terrainVertices.back().x + 20.f, terrainVertices.back().y - 25.f);
	SpawnHole(coordinator, assetManager, { terrainVertices.back().x + 10.f,terrainVertices.back().y + 25.f });
	terrainVertices.emplace_back(terrainVertices.back().x + 20.f, terrainVertices.back().y);

	// Final leveled ground
	terrainVertices.emplace_back(terrainVertices.back().x + 20.f, terrainVertices.back().y + 25.f);
	terrainVertices.emplace_back(terrainVertices.back().x + 450.f, terrainVertices.back().y);

	// Elevated last point
	terrainVertices.emplace_back(terrainVertices.back().x + 800.f, terrainVertices.back().y + 600.f);

	AddObstacles(coordinator, assetManager, terrainVertices);
	// Place colliders on ground surface
	AddTerrain(coordinator, terrainVertices, pcgConfig.elasticity, pcgConfig.friction);

	// From this point don't modify terrainVertices these are used to render ground color


	return terrainVertices;
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
	}

	// Draw the filled polygon representing the terrain
	// Graphics::DrawFillPolygon(screenVertices, color);
	Graphics::DrawFillFadingPolygon(screenVertices, color, Color(Colors::GRAY));

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

void PCG::AddObstacles(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, std::vector<Vector2>& terrainVertices)
{
	// Only spawn it once per map
	bool isBeadsSpawned = false;

	// Skip first and last couple of vertices to avoid ball spawn and the final hole
	for (size_t i = 2; i < terrainVertices.size() - 6; i++)
	{
		if (i % 2 == 0)
		{
			if (Random::Float(0.0f, 1.0f) < .3f)
			{
				SpawnShapes(coordinator, assetManager, terrainVertices[i]);
			}
		}
		else
		{
			// Random chance to spawn an obstacle (30% chance)
			if (Random::Float(0.0f, 1.0f) < .3f)
			{
				// Spawn different obstacles based on random type
				switch (const int obstacleType = Random::Int(1, 3))
				{
					case 1:
						SpawnLaser(coordinator, assetManager, terrainVertices[i], terrainVertices[i + 1]);
						break;
					case 2:
						SpawnPendulum(coordinator, assetManager, terrainVertices[i]);
						break;
					case 3:
						if (!isBeadsSpawned)
						{
							SpawnConnectedBeads(coordinator, assetManager, terrainVertices[i]);
							isBeadsSpawned = true;
						}
						break;
					default:;
				}
			}
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

void PCG::SpawnPendulum(const std::unique_ptr<Coordinator>& coordinator,
	const std::unique_ptr<AssetManager>& assetManager, Vector2 terrainPoint)
{
	// Add sprites for the laser and laser shooter
	assetManager->AddSprite("star", R"(.\Assets\Sprites\Obstacles\star_outline.bmp)", 1, 1);
	assetManager->AddSprite("ball_blue", R"(.\Assets\Sprites\Obstacles\ball_blue.bmp)", 1, 1);

	Entity anchor = coordinator->CreateEntity();
	anchor.AddComponent<TransformComponent>(Vector2(terrainPoint.x, terrainPoint.y + 500.f), Vector2(1.0f, 1.0f));
	anchor.AddComponent<SpriteComponent>("star", 3);
	anchor.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), true, 0.f, 0.f, 0.0f, 1.f, 0.7f);
	anchor.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	anchor.AddComponent<CircleColliderComponent>(assetManager->GetSpriteWidth("star") / 2);
	anchor.Group("Anchor");

	Entity ball = coordinator->CreateEntity();
	ball.AddComponent<TransformComponent>(Vector2(terrainPoint.x, terrainPoint.y + 100.f), Vector2(1.f, 1.0f));
	ball.AddComponent<SpriteComponent>("ball_blue", 3);
	ball.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 5.f, 0.f, 0.0f, 1.f, 0.7f);
	ball.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	ball.AddComponent<CircleColliderComponent>(assetManager->GetSpriteWidth("ball_blue") / 2);
	ball.Group("Spring");
	anchor.AddRelationship(ball, "Spring");

}

void PCG::SpawnConnectedBeads(const std::unique_ptr<Coordinator>& coordinator,
	const std::unique_ptr<AssetManager>& assetManager, Vector2 terrainPoint)
{

	assetManager->AddSprite("star2", R"(.\Assets\Sprites\Obstacles\star_outline.bmp)", 1, 1);
	assetManager->AddSprite("ball_blue2", R"(.\Assets\Sprites\Obstacles\ball_blue.bmp)", 1, 1);

	Entity anchor = coordinator->CreateEntity();
	anchor.AddComponent<TransformComponent>(Vector2(terrainPoint.x, terrainPoint.y + 500.f), Vector2(1.0f, 1.0f));
	anchor.AddComponent<SpriteComponent>("star2", 3);
	anchor.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), true, 0.f, 0.f, 0.0f, 1.f, 0.7f);
	anchor.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	anchor.AddComponent<CircleColliderComponent>(assetManager->GetSpriteWidth("star2") / 2);
	anchor.Group("TightAnchor");


	Entity bead = coordinator->CreateEntity();
	bead.AddComponent<TransformComponent>(Vector2(terrainPoint.x, terrainPoint.y + 100.f), Vector2(1.f, 1.0f));
	bead.AddComponent<SpriteComponent>("ball_blue2", 3);
	bead.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 5.f, 0.f, 0.0f, 1.f, 0.7f);
	bead.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
	bead.AddComponent<CircleColliderComponent>(assetManager->GetSpriteWidth("ball_blue2") / 2);
	bead.Group("Spring");
	anchor.AddRelationship(bead, "Spring");

	std::vector<Entity> joinedEntities;
	joinedEntities.push_back(bead);
	// Constraints
	for (size_t i = 1; i < 3; i++)
	{
		Entity beadSubsequent = coordinator->CreateEntity();
		float x = terrainPoint.x - (static_cast<float>(i) * 60.f);
		beadSubsequent.AddComponent<TransformComponent>(Vector2(x, terrainPoint.y + 100.f), Vector2(1.f, 1.f));
		beadSubsequent.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, static_cast<float>(i), 0.f, 0.0f, 0.1f, 0.1f);
		beadSubsequent.AddComponent<ColliderTypeComponent>(ColliderType::Circle);
		beadSubsequent.AddComponent<CircleColliderComponent>(assetManager->GetSpriteWidth("ball_blue2") / 2);
		beadSubsequent.AddComponent<SpriteComponent>("ball_blue2");
		beadSubsequent.Group("Bead");
		joinedEntities.push_back(beadSubsequent);
	}

	for (size_t i = 0; i < joinedEntities.size() - 1; i++)
	{
		Vector2 midpoint = (joinedEntities[i].GetComponent<TransformComponent>().position + joinedEntities[i + 1].GetComponent<TransformComponent>().position) * 0.5f;
		Entity joint = coordinator->CreateEntity();
		joint.AddComponent<TransformComponent>(midpoint, Vector2(0.1f, 0.1f));
		joint.AddComponent<ConstraintTypeComponent>(ConstrainType::JOINT);
		joint.AddComponent<JointConstraintComponent>(joinedEntities[i], joinedEntities[i + 1]);
	}
}

void PCG::SpawnShapes(const std::unique_ptr<Coordinator>& coordinator, const std::unique_ptr<AssetManager>& assetManager, Vector2& terrainPoint)
{
	// Add sprites
	assetManager->AddSprite("exploding-triangle", R"(.\Assets\Sprites\kenney_physics-assets\Explosive_elements\elementExplosive000.bmp)", 1, 1);
	assetManager->AddSprite("exploding-square", R"(.\Assets\Sprites\kenney_physics-assets\Explosive_elements\elementExplosive014.bmp)", 1, 1);
	assetManager->AddSprite("exploding-rectangle", R"(.\Assets\Sprites\kenney_physics-assets\Explosive_elements\elementExplosive015.bmp)", 1, 1);

	assetManager->AddSprite("glass-triangle", R"(.\Assets\Sprites\kenney_physics-assets\Glass_elements\elementGlass045.bmp)", 1, 1);
	assetManager->AddSprite("glass-square", R"(.\Assets\Sprites\kenney_physics-assets\Glass_elements\elementGlass047.bmp)", 1, 1);
	assetManager->AddSprite("glass-rectangle", R"(.\Assets\Sprites\kenney_physics-assets\Glass_elements\elementGlass048.bmp)", 1, 1);

	// For Filled or hollow
	if (Random::Int(0, 1)) assetManager->AddSprite("wood-triangle", R"(.\Assets\Sprites\kenney_physics-assets\Wood_elements\elementWood054.bmp)", 1, 1);
	else assetManager->AddSprite("wood-triangle", R"(.\Assets\Sprites\kenney_physics-assets\Wood_elements\elementWood002.bmp)", 1, 1);
	if (Random::Int(0, 1)) assetManager->AddSprite("wood-square", R"(.\Assets\Sprites\kenney_physics-assets\Wood_elements\elementWood010.bmp)", 1, 1);
	else assetManager->AddSprite("wood-square", R"(.\Assets\Sprites\kenney_physics-assets\Wood_elements\elementWood026.bmp)", 1, 1);
	if (Random::Int(0, 1)) assetManager->AddSprite("wood-rectangle", R"(.\Assets\Sprites\kenney_physics-assets\Wood_elements\elementWood011.bmp)", 1, 1);
	else assetManager->AddSprite("wood-rectangle", R"(.\Assets\Sprites\kenney_physics-assets\Wood_elements\elementWood027.bmp)", 1, 1);

	if (Random::Int(0, 1)) assetManager->AddSprite("stone-triangle", R"(.\Assets\Sprites\kenney_physics-assets\Stone_elements\elementStone000.bmp)", 1, 1);
	else assetManager->AddSprite("stone-triangle", R"(.\Assets\Sprites\kenney_physics-assets\Stone_elements\elementStone003.bmp)", 1, 1);
	if (Random::Int(0, 1)) assetManager->AddSprite("stone-square", R"(.\Assets\Sprites\kenney_physics-assets\Stone_elements\elementStone011.bmp)", 1, 1);
	else assetManager->AddSprite("stone-square", R"(.\Assets\Sprites\kenney_physics-assets\Stone_elements\elementStone027.bmp)", 1, 1);
	if (Random::Int(0, 1)) assetManager->AddSprite("stone-rectangle", R"(.\Assets\Sprites\kenney_physics-assets\Stone_elements\elementStone012.bmp)", 1, 1);
	else assetManager->AddSprite("stone-rectangle", R"(.\Assets\Sprites\kenney_physics-assets\Stone_elements\elementStone028.bmp)", 1, 1);

	// Random shape
	const auto shape = static_cast<ShapeType>(Random::Int(0, 2));
	// Random material 
	const int material = Random::Int(0, 3);
	const float spriteHeight = assetManager->GetSpriteHeight("stone-square");

	switch (material)
	{
		case 0:
			SpawnStoneShape(coordinator, assetManager, Vector2(terrainPoint.x, terrainPoint.y + spriteHeight), shape);
			break;
		case 1:
			SpawnWoodenShape(coordinator, assetManager, Vector2(terrainPoint.x, terrainPoint.y + spriteHeight), shape);
			break;
		case 2:
			SpawnGlassShape(coordinator, assetManager, Vector2(terrainPoint.x, terrainPoint.y + spriteHeight), shape);
			break;
		case 3:
			SpawnExplosiveShape(coordinator, assetManager, Vector2(terrainPoint.x, terrainPoint.y + spriteHeight), shape);
			break;
		default:;
	}
}

void PCG::SpawnExplosiveShape(const std::unique_ptr<Coordinator>& coordinator,
	const std::unique_ptr<AssetManager>& assetManager, Vector2 spawnPosition, ShapeType shapeType)
{
	// Vector2 spawnPosition = {terrainPoint.x, terrainPoint.y + 10.f };

	// Just for triangle sprites
	const float spriteWidth = assetManager->GetSpriteWidth("exploding-triangle") / 2.f; // Since scaled down by half;
	const float spriteHeight = assetManager->GetSpriteHeight("exploding-triangle") / 2.f;
	std::vector<Vector2> polygonVertices = {
		Vector2(-(spriteWidth / 2.f),  -(spriteHeight / 2.f)), //bottom left
		Vector2((spriteWidth / 2.f),  -(spriteHeight / 2.f)), // bottom right
		Vector2(0.f, (spriteHeight / 2.f)) // Top
	};

	Entity entity = coordinator->CreateEntity();

	switch (shapeType)
	{
		case ShapeType::TRIANGLE:
			entity.AddComponent<SpriteComponent>("exploding-triangle", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 50.f, 0.f, 0.0f, 1.f, 0.7f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Polygon);
			entity.AddComponent<PolygonColliderComponent>(polygonVertices);
			break;
		case ShapeType::SQUARE:
			entity.AddComponent<SpriteComponent>("exploding-square", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 50.f, 0.f, 0.0f, .5f, 0.7f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Box);
			entity.AddComponent<BoxColliderComponent>(assetManager->GetSpriteWidth("exploding-square") / 2, assetManager->GetSpriteHeight("exploding-square") / 2);
			break;
		case ShapeType::RECTANGLE:
			entity.AddComponent<SpriteComponent>("exploding-rectangle", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 50.f, 0.f, 0.0f, .5f, 0.7f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Box);
			entity.AddComponent<BoxColliderComponent>(assetManager->GetSpriteWidth("exploding-rectangle") / 2, assetManager->GetSpriteHeight("exploding-rectangle") / 2);
			break;
	}
	entity.Group("Explosive");
}

void PCG::SpawnGlassShape(const std::unique_ptr<Coordinator>& coordinator,
	const std::unique_ptr<AssetManager>& assetManager, Vector2 spawnPosition, ShapeType shapeType)
{
	// Just for triangle sprites
	const float spriteWidth = assetManager->GetSpriteWidth("exploding-triangle") / 2.f; // Since scaled down by half;
	const float spriteHeight = assetManager->GetSpriteHeight("exploding-triangle") / 2.f;
	std::vector<Vector2> polygonVertices = {
		Vector2(-(spriteWidth / 2.f),  -(spriteHeight / 2.f)), //bottom left
		Vector2((spriteWidth / 2.f),  -(spriteHeight / 2.f)), // bottom right
		Vector2(0.f, (spriteHeight / 2.f)) // Top
	};

	Entity entity = coordinator->CreateEntity();

	switch (shapeType)
	{
		case ShapeType::TRIANGLE:
			entity.AddComponent<SpriteComponent>("glass-triangle", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 5.f, 0.f, 0.0f, 8.f, 0.1f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Polygon);
			entity.AddComponent<PolygonColliderComponent>(polygonVertices);
			break;
		case ShapeType::SQUARE:
			entity.AddComponent<SpriteComponent>("glass-square", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 5.f, 0.f, 0.0f, 8.f, 0.1f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Box);
			entity.AddComponent<BoxColliderComponent>(assetManager->GetSpriteWidth("glass-square") / 2, assetManager->GetSpriteHeight("glass-square") / 2);
			break;
		case ShapeType::RECTANGLE:
			entity.AddComponent<SpriteComponent>("glass-rectangle", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 5.f, 0.f, 0.0f, 8.f, 0.1f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Box);
			entity.AddComponent<BoxColliderComponent>(assetManager->GetSpriteWidth("glass-rectangle") / 2, assetManager->GetSpriteHeight("glass-rectangle") / 2);
			break;
	}
	entity.Group("Glass");
}

void PCG::SpawnWoodenShape(const std::unique_ptr<Coordinator>& coordinator,
	const std::unique_ptr<AssetManager>& assetManager, Vector2 spawnPosition, ShapeType shapeType)
{
	// Just for triangle sprites
	const float spriteWidth = assetManager->GetSpriteWidth("wood-triangle") / 2.f; // Since scaled down by half;
	const float spriteHeight = assetManager->GetSpriteHeight("wood-triangle") / 2.f;
	std::vector<Vector2> polygonVertices = {
		Vector2(-(spriteWidth / 2.f),  -(spriteHeight / 2.f)), //bottom left
		Vector2((spriteWidth / 2.f),  -(spriteHeight / 2.f)), // bottom right
		Vector2(0.f, (spriteHeight / 2.f)) // Top
	};

	Entity entity = coordinator->CreateEntity();

	switch (shapeType)
	{
		case ShapeType::TRIANGLE:
			entity.AddComponent<SpriteComponent>("wood-triangle", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 15.f, 0.f, 0.0f, 3.f, 0.7f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Polygon);
			entity.AddComponent<PolygonColliderComponent>(polygonVertices);
			break;
		case ShapeType::SQUARE:
			entity.AddComponent<SpriteComponent>("wood-square", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 15.f, 0.f, 0.0f, 3.f, 0.7f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Box);
			entity.AddComponent<BoxColliderComponent>(assetManager->GetSpriteWidth("wood-square") / 2, assetManager->GetSpriteHeight("wood-square") / 2);
			break;
		case ShapeType::RECTANGLE:
			entity.AddComponent<SpriteComponent>("wood-rectangle", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 15.f, 0.f, 0.0f, 3.f, 0.7f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Box);
			entity.AddComponent<BoxColliderComponent>(assetManager->GetSpriteWidth("wood-rectangle") / 2, assetManager->GetSpriteHeight("wood-rectangle") / 2);
			break;
	}
	entity.Group("Wood");
}

void PCG::SpawnStoneShape(const std::unique_ptr<Coordinator>& coordinator,
	const std::unique_ptr<AssetManager>& assetManager, Vector2 spawnPosition, ShapeType shapeType)
{
	const float spriteWidth = assetManager->GetSpriteWidth("stone-triangle") / 2.f; // Since scaled down by half;
	const float spriteHeight = assetManager->GetSpriteHeight("stone-triangle") / 2.f;
	std::vector<Vector2> polygonVertices = {
		Vector2(-(spriteWidth / 2.f),  -(spriteHeight / 2.f)), //bottom left
		Vector2((spriteWidth / 2.f),  -(spriteHeight / 2.f)), // bottom right
		Vector2(0.f, (spriteHeight / 2.f)) // Top
	};

	Entity entity = coordinator->CreateEntity();

	switch (shapeType)
	{
		case ShapeType::TRIANGLE:
			entity.AddComponent<SpriteComponent>("stone-triangle", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 50.f, 0.f, 0.0f, 0.2f, 0.9f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Polygon);
			entity.AddComponent<PolygonColliderComponent>(polygonVertices);
			break;
		case ShapeType::SQUARE:
			entity.AddComponent<SpriteComponent>("stone-square", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 50.f, 0.f, 0.0f, 0.2f, 0.9f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Box);
			entity.AddComponent<BoxColliderComponent>(assetManager->GetSpriteWidth("stone-square") / 2, assetManager->GetSpriteHeight("stone-square") / 2);
			break;
		case ShapeType::RECTANGLE:
			entity.AddComponent<SpriteComponent>("stone-rectangle", 3);
			entity.AddComponent<TransformComponent>(spawnPosition, Vector2(.5f, .5f));
			entity.AddComponent<RigidBodyComponent>(Vector2(0.0f, 0.0f), Vector2(), false, 50.f, 0.f, 0.0f, 0.2f, 0.9f);
			entity.AddComponent<ColliderTypeComponent>(ColliderType::Box);
			entity.AddComponent<BoxColliderComponent>(assetManager->GetSpriteWidth("stone-rectangle") / 2, assetManager->GetSpriteHeight("stone-rectangle") / 2);
			break;
	}
	entity.Group("Stone");
}
