#pragma once

#include <string>

#include "src/Utils/Vector2.h"
#include "src/Utils/Color.h"
#include "src/Utils/Font.h"

struct UITextComponent
{
	std::string text;
	Vector2 position;
	Color color;
	FontType font;

	UITextComponent(std::string text, Vector2 position, Color color, FontType font) :
		text(std::move(text)), position(position), color(color), font(font)
	{}
};