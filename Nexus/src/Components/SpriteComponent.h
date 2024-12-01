#pragma once

struct SpriteComponent
{
	int width;
	int height;

	SpriteComponent(int width = 0, int height = 0) : width(width), height(height)
	{
		// TODO: Should keep track of sprite asset location
	}
};