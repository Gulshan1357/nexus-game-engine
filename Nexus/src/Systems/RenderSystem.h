#pragma once

#include <algorithm> // For sorting
#include <iostream>

#include "../App/app.h"
#include "../App/SimpleSprite.h"

#include "src/ECS/Entity.h"
#include "src/ECS/System.h"
#include "src/AssetManagement/AssetManager.h"
#include "src/Components/AnimationComponent.h"

#include "src/Components/SpriteComponent.h"
#include "src/Components/TransformComponent.h"


class RenderSystem : public System
{
public:
	RenderSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(const std::unique_ptr<AssetManager>& assetManager) const
	{
		//------------------------------------------------------------------------
		// To make sure that the entity with higher SpriteComponent::z-index are rendered on top of lower, we need a sorted vector of SpriteComponent w.r.t z-index. 
		// I decided to create a struct of transform component, add z-index sorted entities into it and render based on it.
		// TODO: Think of a way to avoid the sorting every update()
		//------------------------------------------------------------------------

		struct RenderableEntity
		{
			const TransformComponent* transformComponent;
			const SpriteComponent* spriteComponent;
			// Need this to decided if the render system should handle setting the frame or animation system
			bool bHasAnimationComponent; // TODO: Refactor this.
		};

		std::vector<RenderableEntity> renderQueue;

		const auto entities = GetSystemEntities();
		renderQueue.reserve(entities.size());

		// Debug
		std::string text = "For Debug!";

		// Entity& testWaterEntity = entities[0];
		for (const auto& entity : entities)
		{
			renderQueue.emplace_back(RenderableEntity{
				&entity.GetComponent<TransformComponent>(),
				&entity.GetComponent<SpriteComponent>(),
				entity.HasComponent<AnimationComponent>()
				});

			// Debug
			if (entity.HasTag("Player2"))
			{
				text = "Player2 velocity: " + entity.GetComponent<RigidBodyComponent>().velocity.ToString();
			}
		}

		// Sort the vector by z-index. If the entity order turned out to be important later, then use stable_sort()
		std::sort(
			renderQueue.begin(),
			renderQueue.end(),
			[](const RenderableEntity& a, const RenderableEntity& b)
			{
				return a.spriteComponent->zIndex < b.spriteComponent->zIndex;
			}
		);

		// Finally render the entities based on sorted renderQueue
		for (const auto& [transformComponent, spriteComponent, bHasAnimationComponent] : renderQueue)
		{
			// const auto& transformComponent = *transformComponent;
			// const auto& spriteComponent = *spriteComponent;

			CSimpleSprite* sprite = assetManager->GetSprite(spriteComponent->assetId);
			sprite->SetPosition(transformComponent->position.x, transformComponent->position.y);
			sprite->SetAngle(transformComponent->rotation);
			sprite->SetScale(transformComponent->scale.x);

			// Usually the animation component handles which frame to render from the sprite but in absence of it setting the frame here
			if (!bHasAnimationComponent)
			{
				sprite->SetFrame(spriteComponent->frame);
			}

			sprite->Draw();
		}

		// Debug
		App::Print(50.f, 50.f, text.c_str(), 1, 0, 0, GLUT_BITMAP_HELVETICA_12);
	}
};