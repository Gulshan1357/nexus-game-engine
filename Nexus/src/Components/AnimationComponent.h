#pragma once

struct AnimationComponent
{
	// Set this to true if you want the animation system to play this entity's animation. 
	bool bIsActive;
	// Total number of frames in the animation
	int animationLength;
	// Animation system deactivate this animation after it reaches the end of animation
	bool bIsLooping;
	// row number of the animation sprite. For player animation, input system modify this value so that animation system can play different animations for different actions
	int animationFramesRow;
	bool playFromBeginning;

	explicit AnimationComponent(const bool bIsActive = false, const int animationLength = 2, const bool bIsLooping = true, const int animationFramesRow = 0, const bool playFromBeginning = false) :
		bIsActive(bIsActive), animationLength(animationLength), bIsLooping(bIsLooping), animationFramesRow(animationFramesRow), playFromBeginning(playFromBeginning)
	{}
};