#pragma once

#include <algorithm> // For sorting

#include "../App/app.h"
#include "../App/SimpleSprite.h"

#include "src/ECS/Entity.h"
#include "src/ECS/System.h"
#include "src/AssetManagement/AssetManager.h"

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

	void Update(std::unique_ptr<AssetManager>& assetManager)
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
		};

		std::vector<RenderableEntity> renderQueue;

		auto entities = GetSystemEntities();
		renderQueue.reserve(entities.size());

		for (const auto& entity : entities)
		{
			renderQueue.emplace_back(RenderableEntity{
				&entity.GetComponent<TransformComponent>(),
				&entity.GetComponent<SpriteComponent>()
				});
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
		for (const auto& renderableEntity : renderQueue)
		{
			const auto& transformComponent = *renderableEntity.transformComponent;
			const auto& spriteComponent = *renderableEntity.spriteComponent;

			CSimpleSprite* sprite = assetManager->GetSprite(spriteComponent.assetId);
			sprite->SetPosition(transformComponent.position.x, transformComponent.position.y);
			sprite->SetAngle(transformComponent.rotation);
			sprite->SetScale(transformComponent.scale.x);
			sprite->Draw();
		}
	}
};