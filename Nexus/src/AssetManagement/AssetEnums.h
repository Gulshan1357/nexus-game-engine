#pragma once

namespace Asset
{
	// Demo sprite
	enum DemoPlayer
	{
		ANIM_FORWARDS = 0,
		ANIM_BACKWARDS = 1,
		ANIM_LEFT = 2,
		ANIM_RIGHT = 3,
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