#pragma once

enum class ColliderType
{
	Box,
	Circle,
	Polygon,
};

/**
 * ColliderType Component provide the information regarding the shape of collier for Collision System
 * @param type ColliderType Enum with possible values = {Box, Circle, Polygon}
*/
struct ColliderTypeComponent
{
	ColliderType type;

	explicit ColliderTypeComponent(ColliderType type) : type(type) {}
};