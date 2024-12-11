#pragma once

#include <string>

struct SpriteComponent
{
	std::string assetId;
	unsigned int frame; // Frame to render when not animating
	int zIndex;

	SpriteComponent(std::string assetId = "", unsigned int frame = 0, int zIndex = 0) :
		assetId(assetId), frame(frame), zIndex(zIndex)
	{}
};