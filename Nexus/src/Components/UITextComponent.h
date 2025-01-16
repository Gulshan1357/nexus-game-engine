#pragma once

#include <string>

#include "src/Utils/Vector2.h"
#include "src/Utils/Color.h"
#include "src/Utils/Font.h"

/**
 * UITextComponent Component provides information for RenderText System
 * @param text(string) Text to render
 * @param position(Vector2) Position from bottom-left
 * @param color(Color) RGB color values
 * @param font(FontType enum) Select a font the list of available fonts in FontUtils::FontType enum
 * @param isWorldSpace(bool) Default is false. If set to true the position is relative to world. Otherwise the position is relative to the camera.
*/
struct UITextComponent
{
	std::string text;
	Vector2 position;
	Color color;
	FontType font;
	bool isWorldSpace = false;

	UITextComponent(
		std::string text,
		const Vector2 position,
		const Color color,
		const FontType font,
		const bool isWorldSpace
	) :
		text(std::move(text)), position(position), color(color), font(font), isWorldSpace(isWorldSpace)
	{}
};