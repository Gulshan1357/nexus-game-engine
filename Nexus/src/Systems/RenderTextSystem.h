#pragma once

#include "App/app.h"

#include "src/ECS/Entity.h"
#include "src/ECS/System.h"

#include "src/Components/UITextComponent.h"

#include "src/Utils/Font.h"

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
			App::Print(uiText.position.x, uiText.position.y, uiText.text.c_str(), uiText.color.r, uiText.color.g, uiText.color.b, FontUtils::GetFontPointer(uiText.font));
		}
	}
};
