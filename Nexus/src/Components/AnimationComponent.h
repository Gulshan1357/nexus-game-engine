#pragma once

struct AnimationComponent
{
	bool bIsPlaying;
	int animationFramesRow; // row number of the animation sprite
	bool playFromBeginning;

	AnimationComponent(bool bIsPlaying = false, int animationFramesRow = 0, bool playFromBeginning = false) :
		bIsPlaying(bIsPlaying), animationFramesRow(animationFramesRow), playFromBeginning(playFromBeginning)
	{}
};