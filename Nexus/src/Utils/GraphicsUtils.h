#pragma once

#include "../App/app.h"

#include "Vector2.h"
#include "Color.h"

namespace Graphics
{
	//-------------------------------------------------------------------------------------------
	// Print text to screen at Vector2(x,y) coordinates using color r = red, g = green, b=blue using the passed GLUT font. 
	// Color values are in the range 0.0f to 1.0f. Default color is white
	// Available fonts...
	// GLUT_BITMAP_9_BY_15, GLUT_BITMAP_8_BY_13, GLUT_BITMAP_TIMES_ROMAN_10, GLUT_BITMAP_TIMES_ROMAN_24
	// GLUT_BITMAP_HELVETICA_10, GLUT_BITMAP_HELVETICA_12, GLUT_BITMAP_HELVETICA_18	
	//-------------------------------------------------------------------------------------------
	static void PrintText(const std::string& text, const Vector2& location, const Color color = Color(), void* font = GLUT_BITMAP_HELVETICA_12)
	{
		App::Print(location.x, location.y, text.c_str(), color.r, color.g, color.b, font);
	}

	//-------------------------------------------------------------------------------------------
	// Draw a line from startPoint(x, y) to endPoint(x, y) and a radius using color r = red, g = green, b=blue 
	// Color values are in the range 0.0f to 1.0f. Default color is white
	//-------------------------------------------------------------------------------------------
	static void DrawLine(const Vector2& startPoint, const Vector2& endPoint, const Color color = Color())
	{
		App::DrawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, color.r, color.g, color.b);
	}

	//-------------------------------------------------------------------------------------------
	// Draw a circle with center at Vector2(x,y) and a radius using color r = red, g = green, b=blue 
	// Color values are in the range 0.0f to 1.0f. Default color is white
	// Default Segment = 36
	//-------------------------------------------------------------------------------------------
	static void DrawCircle(const Vector2& center, float radius, const int segments = 36, const Color color = Color())
	{
		for (int i = 0; i < segments; ++i)
		{
			const float theta1 = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
			const float theta2 = 2.0f * PI * static_cast<float>(i + 1) / static_cast<float>(segments);

			const Vector2 p1 = center + Vector2(radius * cosf(theta1), radius * sinf(theta1));
			const Vector2 p2 = center + Vector2(radius * cosf(theta2), radius * sinf(theta2));

			DrawLine(p1, p2, color);
		}
	}

	//-------------------------------------------------------------------------------------------
	// Draw a polygon based on the vertices(vector<Vector2>) using color r = red, g = green, b=blue 
	// Color values are in the range 0.0f to 1.0f. Default color is white
	//-------------------------------------------------------------------------------------------
	static void DrawPolygon(const std::vector<Vector2>& vertices, const Color color = Color())
	{
		if (vertices.empty()) return;

		for (size_t i = 0; i < vertices.size(); ++i)
		{
			const auto& v1 = vertices[i];
			const auto& v2 = vertices[(i + 1) % vertices.size()];
			DrawLine(v1, v2, color); // Use DrawLine to connect the vertices
		}
	}
}