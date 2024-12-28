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

	void Update() const
	{
		for (auto entity : GetSystemEntities())
		{
			const UITextComponent& uiText = entity.GetComponent<UITextComponent>();
			Graphics::PrintText(uiText.text, uiText.position, uiText.color);
		}
	}
};
