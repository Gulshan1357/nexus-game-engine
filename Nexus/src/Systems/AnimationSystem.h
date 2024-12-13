#pragma once

#include <iostream>
#include <memory>

#include "../App/app.h"
#include "../App/SimpleSprite.h"

#include "src/ECS/System.h"
#include "src/ECS/Entity.h"

#include "src/AssetManagement/AssetManager.h"
#include "src/EventManagement/EventManager.h"

#include "src/Events/InputKeyEvent.h"

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

	// For animating player
	void SubscribeToEvents(const std::unique_ptr<EventManager>& eventManager)
	{
		using CallbackType = std::function<void(AnimationSystem*, KeyPressEvent&)>;
		CallbackType callback = std::bind(&AnimationSystem::OnKeyPressed, this, std::placeholders::_2);
		eventManager->SubscribeToEvent<KeyPressEvent>(this, callback);
	}

	// Changing player animation based on input
	void OnKeyPressed(const KeyPressEvent& event)
	{
		//TODO: Change animation based on rigidbody velocity.
		auto& animationComponent = event.player.GetComponent<AnimationComponent>();
		auto& spriteComponent = event.player.GetComponent<SpriteComponent>();

		switch (event.action)
		{
			case Input::PlayerAction::MOVE_UP:
				animationComponent.animationFramesRow = Asset::ANIM_FORWARDS;
				spriteComponent.frame = Asset::ANIM_FORWARDS;
				break;
			case Input::PlayerAction::MOVE_RIGHT:
				animationComponent.animationFramesRow = Asset::ANIM_RIGHT;
				spriteComponent.frame = Asset::ANIM_RIGHT;
				break;
			case Input::PlayerAction::MOVE_DOWN:
				animationComponent.animationFramesRow = Asset::ANIM_BACKWARDS;
				spriteComponent.frame = Asset::ANIM_BACKWARDS;
				break;
			case Input::PlayerAction::MOVE_LEFT:
				animationComponent.animationFramesRow = Asset::ANIM_LEFT;
				spriteComponent.frame = Asset::ANIM_LEFT;
				break;
			default:
				break;
		}
	}

	void Update(std::unique_ptr<AssetManager>& assetManager, float deltaTime)
	{
		for (auto entity : GetSystemEntities())
		{
			auto& animationComponent = entity.GetComponent<AnimationComponent>();
			auto& spriteComponent = entity.GetComponent<SpriteComponent>();

			if (CSimpleSprite* sprite = assetManager->GetSprite(spriteComponent.assetId))
			{
				if (animationComponent.bIsPlaying)
				{
					sprite->Update(deltaTime);
					sprite->SetAnimation(animationComponent.animationFramesRow, animationComponent.playFromBeginning);
				}
				else
				{
					sprite->SetFrame(spriteComponent.frame);
				}
			}
		}
	}
};