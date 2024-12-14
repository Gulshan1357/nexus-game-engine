#pragma once

#include <string>

struct SpriteComponent
{
	std::string assetId;
	unsigned int frame; // Frame to render when not animating
	int zIndex;

	explicit SpriteComponent(std::string assetId = "", const unsigned int frame = 0, const int zIndex = 0) :
		assetId(std::move(assetId)), frame(frame), zIndex(zIndex)
	{}
};