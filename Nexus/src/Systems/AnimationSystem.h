#pragma once

#include <iostream>
#include <memory>

#include "../App/app.h"
#include "../App/SimpleSprite.h"

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/AssetManagement/AssetManager.h"

#include "src/Components/SpriteComponent.h"
#include "src/Components/AnimationComponent.h"

class AnimationSystem : public System
{
public:
	AnimationSystem()
	{
		RequireComponent<SpriteComponent>();
		RequireComponent<AnimationComponent>();
	}

	void Update(std::unique_ptr<AssetManager>& assetManager, float deltaTime)
	{
		for (auto entity : GetSystemEntities())
		{
			auto& animationComponent = entity.GetComponent<AnimationComponent>();
			auto& spriteComponent = entity.GetComponent<SpriteComponent>();

			if (CSimpleSprite* sprite = assetManager->GetSprite(spriteComponent.assetId))
			{
				if (animationComponent.bIsActive)
				{
					sprite->Update(deltaTime);
					sprite->SetAnimation(animationComponent.animationFramesRow, animationComponent.playFromBeginning);
				}
				else
				{
					sprite->SetFrame(spriteComponent.frame);
				}

				// If the animationComponent::bIsLooping is false then stop the animation when it reaches the last frame
				if (!animationComponent.bIsLooping && (sprite->GetFrame() + 1) % animationComponent.animationLength == 0)
				{
					animationComponent.bIsActive = false;
					sprite->SetFrame(spriteComponent.frame);
				}
			}
		}
	}
};