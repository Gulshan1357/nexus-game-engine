#pragma once

// Following are the available fonts as per App.h
// GLUT_BITMAP_9_BY_15, GLUT_BITMAP_8_BY_13, GLUT_BITMAP_TIMES_ROMAN_10, GLUT_BITMAP_TIMES_ROMAN_24
// GLUT_BITMAP_HELVETICA_10, GLUT_BITMAP_HELVETICA_12, GLUT_BITMAP_HELVETICA_18	

// The enum can be mapped to GLUT font pointer using FontUtils::GetFontPointer(FontType)
enum class FontType
{
	BITMAP_9_BY_15,
	BITMAP_8_BY_13,
	TIMES_ROMAN_10,
	TIMES_ROMAN_24,
	HELVETICA_10,
	HELVETICA_12,
	HELVETICA_18
};

// Function to map FontType to the actual font pointer of type void*.
namespace FontUtils
{
	inline void* GetFontPointer(const FontType fontType)
	{
		switch (fontType)
		{
			case FontType::BITMAP_9_BY_15: return GLUT_BITMAP_9_BY_15;
			case FontType::BITMAP_8_BY_13: return GLUT_BITMAP_8_BY_13;
			case FontType::TIMES_ROMAN_10: return GLUT_BITMAP_TIMES_ROMAN_10;
			case FontType::TIMES_ROMAN_24: return GLUT_BITMAP_TIMES_ROMAN_24;
			case FontType::HELVETICA_10:   return GLUT_BITMAP_HELVETICA_10;
			case FontType::HELVETICA_12:   return GLUT_BITMAP_HELVETICA_12;
			case FontType::HELVETICA_18:   return GLUT_BITMAP_HELVETICA_18;
			default: return GLUT_BITMAP_HELVETICA_18;
		}
	}
}