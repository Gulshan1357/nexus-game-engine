#pragma once

#include <string>

/**
 * SpriteComponent Component provides information for Render System
 * @param assetId Asset Manager stores the sprite location using assetID string. This is a reference to that
 * @param zIndex The entities with higher sprite z-index will be rendered on top of entities with lower z-index
 * @param frame Frame to render when not animating
*/
struct SpriteComponent
{
	std::string assetId;
	int zIndex;
	unsigned int frame;

	explicit SpriteComponent(std::string assetId = "", const int zIndex = 0, const unsigned int frame = 0) :
		assetId(std::move(assetId)), zIndex(zIndex), frame(frame)
	{}
};