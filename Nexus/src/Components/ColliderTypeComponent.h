#pragma once

#include "src/Physics/Contact.h"

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

	std::vector<Contact> contacts; // Store contact info for Render debug system

	explicit ColliderTypeComponent(const ColliderType type)
		: type(type), contacts({})
	{}
};