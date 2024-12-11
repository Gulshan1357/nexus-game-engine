#pragma once

namespace Asset
{
	// Demo sprite
	enum DemoPlayer
	{
		ANIM_BACKWARDS = 0,
		ANIM_LEFT = 8,
		ANIM_RIGHT = 16,
		ANIM_FORWARDS = 24,
	};

	// Tile map cols and rows = 21 X 8
	enum Tiles
	{
		GRASS = 0,
		DESERT = 1,
		WATER = 5,
		LAVA = 6,
		LOCK = 130,
		KEY = 152,
	};
}