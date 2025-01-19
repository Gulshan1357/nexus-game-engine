#include "stdafx.h"

#include "PCG.h"

#include "src/ECS/Coordinator.h"
#include "src/ECS/Entity.h"

#include "src/Components/ColliderTypeComponent.h"
#include "src/Components/PolygonColliderComponent.h"
#include "src/Components/RigidBodyComponent.h"
#include "src/Components/TransformComponent.h"

#include "src/Physics/Camera.h"
#include "src/Utils/Color.h"
#include "src/Utils/GraphicsUtils.h"

void PCG::AddTerrain(const std::unique_ptr<Coordinator>& coordinator, const std::vector<Vector2>& terrainVertices, const float elasticity, const float friction)
{
	for (size_t i = 0; i < terrainVertices.size() - 1; i++)
	{
		const Vector2& currentPos = terrainVertices[i];
		const Vector2& nextPos = terrainVertices[i + 1];
		float xDifference = nextPos.x - currentPos.x;
		float yDifference = nextPos.y - currentPos.y;
		const float colliderDepth = 500.f;

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