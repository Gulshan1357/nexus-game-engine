#pragma once

#include <vector>

#include "src/Utils/Vector2.h"

/**
 * PolygonColliderComponent Polygon provides information for Collision System
 * @param localVertices (vector<Vector2>) of the PolygonCollider with respect to the local space where origin is the position of entity
 * @param offset (Vector2)
*/
struct PolygonColliderComponent
{
	// Local space Vertices
	std::vector<Vector2> localVertices;
	// Global space vertices. These will be calculated by the Physics system on every update. These coordinates are the result of torque(angular velocity and angular acceleration) and offset.
	std::vector<Vector2> globalVertices;
	Vector2 offset;

	explicit PolygonColliderComponent(const std::vector<Vector2>& localVertices, const Vector2 offset = Vector2()) :
		localVertices(localVertices), offset(offset)
	{
		// Ensure globalVertices has the same size as localVertices
		globalVertices.resize(localVertices.size());
	}
};