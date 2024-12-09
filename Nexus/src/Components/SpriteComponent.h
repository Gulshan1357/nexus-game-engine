#pragma once

#include <string>

struct SpriteComponent
{
	std::string assetId;
	int zIndex;

	SpriteComponent(std::string assetId = "", int zIndex = 0) :
		assetId(assetId), zIndex(zIndex)
	{}
};