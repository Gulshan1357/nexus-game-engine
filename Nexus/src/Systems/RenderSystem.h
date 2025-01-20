#pragma once

#include <algorithm>

#include "../App/SimpleSprite.h"

#include "src/ECS/Entity.h"
#include "src/ECS/System.h"
#include "src/AssetManagement/AssetManager.h"

#include "src/Components/AnimationComponent.h"
#include "src/Components/SpriteComponent.h"
#include "src/Components/TransformComponent.h"

#include "src/Physics/Camera.h"

class RenderSystem : public System
{
public:
	RenderSystem()
	{
		RequireComponent<TransformComponent>();
		RequireComponent<SpriteComponent>();
	}

	void Update(const std::unique_ptr<AssetManager>& assetManager, const Camera& camera) const
	{
		//------------------------------------------------------------------------
		// To make sure that the entity with higher SpriteComponent::z-index are rendered on top of lower, we need a sorted vector of SpriteComponent w.r.t z-index. 
		// I decided to create a struct of transform component, add z-index sorted entities into it and render based on it.
		// TODO: Think of a way to avoid the sorting every update()
		//------------------------------------------------------------------------

		const auto& entities = GetSystemEntities();

		m_renderQueue.clear();
		m_renderQueue.reserve(entities.size());

		for (const auto& entity : entities)
		{
			m_renderQueue.emplace_back(RenderableEntity{
				&entity.GetComponent<TransformComponent>(),
				&entity.GetComponent<SpriteComponent>(),
				entity.HasComponent<AnimationComponent>()
				});
		}

		// Sort the vector by z-index. If the entity order turned out to be important later, then use stable_sort()
		std::sort(
			m_renderQueue.begin(),
			m_renderQueue.end(),
			[](const RenderableEntity& a, const RenderableEntity& b)
			{
				return a.spriteComponent->zIndex < b.spriteComponent->zIndex;
			}
		);

		// RenderTerrain the entities based on sorted renderQueue
		for (const auto& [transformComponent, spriteComponent, bHasAnimationComponent] : m_renderQueue)
		{
			CSimpleSprite* sprite = assetManager->GetSprite(spriteComponent->assetId);
			if (!sprite) continue;

			// Transform position through camera
			const Vector2 screenPos = Camera::WorldToScreen(transformComponent->position, camera);

			// sprite->SetPosition(transformComponent->position.x, transformComponent->position.y);
			sprite->SetPosition(screenPos.x, screenPos.y);
			sprite->SetAngle(transformComponent->rotation);
			sprite->SetScale(transformComponent->scale.x);

			// Usually the animation component handles which frame to render from the sprite but in absence of it setting the frame here
			if (!bHasAnimationComponent)
			{
				sprite->SetFrame(spriteComponent->frame);
			}

			sprite->Draw();
		}
	}

private:
	struct RenderableEntity
	{
		const TransformComponent* transformComponent;
		const SpriteComponent* spriteComponent;
		// Need to decided if the render system should handle setting the frame or animation system
		bool bHasAnimationComponent; // TODO: Refactor this.
	};

	// Cache for transformed positions to avoid recalculating for static objects
	mutable std::vector<RenderableEntity> m_renderQueue;
};
