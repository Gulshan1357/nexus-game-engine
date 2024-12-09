#pragma once

#include "../App/app.h"
#include "../App/SimpleSprite.h"

#include "src/ECS/Entity.h"
#include "src/ECS/System.h"

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

	void Update()
	{
		for (auto entity : GetSystemEntities())
		{
			const TransformComponent& transform = entity.GetComponent<TransformComponent>();
			const SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();

			// TODO: Should render based on the sprite location inside SpriteComponent
			CSimpleSprite* test2Sprite = App::CreateSprite(".\\TestData\\Test.bmp", 8, 4);
			test2Sprite->SetPosition(transform.position.x, transform.position.y);
			test2Sprite->Draw();
		}
	}
};