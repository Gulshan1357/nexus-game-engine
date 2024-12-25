#pragma once

#include "src/Utils/Vector2.h"

/**
 * BoxColliderComponent Component provides information for Collision System
 * @param width (float)  of the BoxCollider
 * @param height (float) of the BoxCollider
 * @param offset (Vector2)
*/
struct BoxColliderComponent
{
	float width;
	float height;
	Vector2 offset;

	std::vector<Vector2> localVertices;  // Vertices in local space
	std::vector<Vector2> globalVertices; // Transformed vertices in global space

	explicit BoxColliderComponent(const float width = 0, const float height = 0, const Vector2 offset = Vector2())
		: width(width), height(height), offset(offset)
	{
		// Initialize local vertices as a box centered around (0, 0) with the given width and height
		localVertices = {
			Vector2(-width / 2, -height / 2),
			Vector2(width / 2, -height / 2),
			Vector2(width / 2, height / 2),
			Vector2(-width / 2, height / 2)
		};
		globalVertices.resize(localVertices.size());
	}
};