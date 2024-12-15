#pragma once

/**
 * Animation Component provides information for Animation System
 * @param bIsActive Set this to true if you want the animation system to play this entity's animation. 
 * @param animationLength Total number of frames in the animation
 * @param bIsLooping If set to false, the Animation System will set bIsActive to false after the animation is completed.
 * @param animationFramesRow Specifies the row of the sprite sheet to use for animation. Changing this value switches to a different animation row.
 * @param playFromBeginning  Determines whether the animation should start playing from the beginning. Set this to true to restart the animation.
*/
struct AnimationComponent
{
	bool bIsActive;
	int animationLength;
	bool bIsLooping;
	int animationFramesRow;
	bool playFromBeginning;

	explicit AnimationComponent(const bool bIsActive = false, const int animationLength = 2, const bool bIsLooping = true, const int animationFramesRow = 0, const bool playFromBeginning = false) :
		bIsActive(bIsActive), animationLength(animationLength), bIsLooping(bIsLooping), animationFramesRow(animationFramesRow), playFromBeginning(playFromBeginning)
	{}
};