#pragma once

#include <string>

#include "../Utils/Vector2.h"
#include "../Utils/Color.h"
#include "../Utils/Font.h"

struct UITextComponent
{
	std::string text;
	Vector2 position;
	Color color;
	FontType font;

	UITextComponent(std::string text, Vector2 position, Color color, FontType font) : text(std::move(text)), position(position), color(color), font(font) {}
};