#pragma once

#include "App/app.h"

#include "src/ECS/Entity.h"
#include "src/ECS/System.h"

#include "src/Components/UITextComponent.h"

#include "src/Utils/Font.h"
#include "src/Utils/GraphicsUtils.h"

class RenderTextSystem : public System
{
public:
	RenderTextSystem()
	{
		RequireComponent<UITextComponent>();
	}

	void Update(const Camera& camera) const
	{
		for (auto entity : GetSystemEntities())
		{
			const UITextComponent& uiText = entity.GetComponent<UITextComponent>();

			Vector2 screenPos;
			if (uiText.isWorldSpace)
			{
				screenPos = Camera::WorldToScreen(uiText.position, camera);
			}
			else
			{
				screenPos = uiText.position;
			}

			Graphics::PrintText(uiText.text, screenPos, uiText.color);
		}
	}
};
