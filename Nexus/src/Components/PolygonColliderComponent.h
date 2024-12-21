#pragma once

#include <vector>

#include "src/Utils/Vector2.h"

/**
 * PolygonColliderComponent Polygon provides information for Collision System
 * @param vertices (vector<Vector2>) of the PolygonCollider
 * @param offset (Vector2)
*/
struct PolygonColliderComponent
{
	std::vector<Vector2> vertices;
	Vector2 offset;

	explicit PolygonColliderComponent(const std::vector<Vector2>& vertices, const Vector2 offset = Vector2()) :
		vertices(vertices), offset(offset)
	{}
};