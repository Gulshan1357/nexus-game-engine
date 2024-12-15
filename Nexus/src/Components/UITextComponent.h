#pragma once

#include <string>

#include "src/Utils/Vector2.h"
#include "src/Utils/Color.h"
#include "src/Utils/Font.h"

/**
 * UITextComponent Component provides information for RenderText System
 * @param text Text to render
 * @param position Position from bottom-left
 * @param color RGB color values
 * @param font Select a font the list of available fonts in FontUtils::FontType enum
*/
struct UITextComponent
{
	std::string text;
	Vector2 position;
	Color color;
	FontType font;

	UITextComponent(std::string text, const Vector2 position, const Color color, const FontType font) :
		text(std::move(text)), position(position), color(color), font(font)
	{}
};