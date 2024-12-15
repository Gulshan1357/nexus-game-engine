#pragma once

#include <string>

/**
 * SpriteComponent Component provides information for Render System
 * @param assetId Asset Manager stores the sprite location using assetID string. This is a reference to that
 * @param frame Frame to render when not animating
 * @param zIndex The entities with higher sprite z-index will be rendered on top of entities with lower z-index
*/
struct SpriteComponent
{
	std::string assetId;
	unsigned int frame; 
	int zIndex;

	explicit SpriteComponent(std::string assetId = "", const unsigned int frame = 0, const int zIndex = 0) :
		assetId(std::move(assetId)), frame(frame), zIndex(zIndex)
	{}
};