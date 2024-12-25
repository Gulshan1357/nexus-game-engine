#pragma once

#include "../App/app.h"

#include "Vector2.h"

namespace Graphics
{
	//-------------------------------------------------------------------------------------------
	// Print text to screen at Vector2(x,y) coordinates using color r = red, g = green, b=blue using the passed GLUT font. 
	// Color values are in the range 0.0f to 1.0f. Default color is black
	// Available fonts...
	// GLUT_BITMAP_9_BY_15, GLUT_BITMAP_8_BY_13, GLUT_BITMAP_TIMES_ROMAN_10, GLUT_BITMAP_TIMES_ROMAN_24
	// GLUT_BITMAP_HELVETICA_10, GLUT_BITMAP_HELVETICA_12, GLUT_BITMAP_HELVETICA_18	
	//-------------------------------------------------------------------------------------------
	static void PrintText(const std::string& text, const Vector2& location, const float r = 0.0f, const float g = 0.0f, const float b = 0.0f, void* font = GLUT_BITMAP_HELVETICA_12)
	{
		App::Print(location.x, location.y, text.c_str(), r, g, b, font);
	}

	//-------------------------------------------------------------------------------------------
	// Draw a line from startPoint(x, y) to endPoint(x, y) and a radius using color r = red, g = green, b=blue 
	// Color values are in the range 0.0f to 1.0f. Default color is white
	//-------------------------------------------------------------------------------------------
	static void DrawLine(const Vector2& startPoint, const Vector2& endPoint, const float r = 1.0f, const float g = 1.0f, const float b = 1.0f)
	{
		App::DrawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, r, g, b);
	}

	//-------------------------------------------------------------------------------------------
	// Draw a circle with center at Vector2(x,y) and a radius using color r = red, g = green, b=blue 
	// Color values are in the range 0.0f to 1.0f. Default color is white
	// Default Segment = 36
	//-------------------------------------------------------------------------------------------
	static void DrawCircle(const Vector2& center, float radius, const int segments = 36, const float r = 1.0f, const float g = 1.0f, const float b = 1.0f)
	{
		for (int i = 0; i < segments; ++i)
		{
			const float theta1 = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
			const float theta2 = 2.0f * PI * static_cast<float>(i + 1) / static_cast<float>(segments);

			const Vector2 p1 = center + Vector2(radius * cosf(theta1), radius * sinf(theta1));
			const Vector2 p2 = center + Vector2(radius * cosf(theta2), radius * sinf(theta2));

			DrawLine(p1, p2, r, g, b);
		}
	}

	//-------------------------------------------------------------------------------------------
	// Draw a polygon based on the vertices(vector<Vector2>) using color r = red, g = green, b=blue 
	// Color values are in the range 0.0f to 1.0f. Default color is white
	//-------------------------------------------------------------------------------------------
	static void DrawPolygon(const std::vector<Vector2>& vertices, const float r = 1.0f, const float g = 1.0f, const float b = 1.0f)
	{
		if (vertices.empty()) return;

		for (size_t i = 0; i < vertices.size(); ++i)
		{
			const auto& v1 = vertices[i];
			const auto& v2 = vertices[(i + 1) % vertices.size()];
			DrawLine(v1, v2, r, g, b); // Use DrawLine to connect the vertices
		}
	}
}