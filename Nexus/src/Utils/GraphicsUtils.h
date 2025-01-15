#pragma once

#include "../App/app.h"

#include <algorithm>

#include "Vector2.h"
#include "Color.h"

namespace Graphics
{
	//-------------------------------------------------------------------------------------------
	// Print text to screen at Vector2(x,y) coordinates using the passed GLUT font. 
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
	// Draw a line from startPoint(x, y) to endPoint(x, y)
	// Color values are in the range 0.0f to 1.0f. Default color is white
	//-------------------------------------------------------------------------------------------
	static void DrawLine(const Vector2& startPoint, const Vector2& endPoint, const Color color = Color())
	{
		App::DrawLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y, color.r, color.g, color.b);
	}

	//-------------------------------------------------------------------------------------------
	// Draw a circle outline with center at Vector2(x,y)
	// Color values are in the range 0.0f to 1.0f. Default color is white
	// Default Segment = 36
	//-------------------------------------------------------------------------------------------
	static void DrawCircle(const Vector2& center, const float radius, const int segments = 36, const Color color = Color())
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
	// Draw a polygon outline based on the vertices(vector<Vector2>)
	// Color values are in the range 0.0f to 1.0f. Default color is white
	//-------------------------------------------------------------------------------------------
	static void DrawPolygon(const std::vector<Vector2>& vertices, const Color color = Color())
	{
		if (vertices.size() < 3) return;

		for (size_t i = 0; i < vertices.size(); ++i)
		{
			const auto& v1 = vertices[i];
			const auto& v2 = vertices[(i + 1) % vertices.size()];
			DrawLine(v1, v2, color); // Use DrawLine to connect the vertices
		}
	}

	//-------------------------------------------------------------------------------------------
	// Draw a rectangle outline with the specified bottom-left corner, width, height, and color
	// Color values are in the range 0.0f to 1.0f. Default color is white
	//-------------------------------------------------------------------------------------------
	inline void DrawRectangle(const Vector2& bottomLeft, const float width, const float height, const Color& color = Color())
	{
		// Define the rectangle's vertices
		std::vector<Vector2> vertices = {
			bottomLeft,
			{bottomLeft.x + width, bottomLeft.y},
			{bottomLeft.x + width, bottomLeft.y + height},
			{bottomLeft.x, bottomLeft.y + height}
		};

		// Rectangle outline
		DrawPolygon(vertices, color);
	}

	//-------------------------------------------------------------------------------------------
	// Draw a filled circle with center at Vector2(x,y)
	// Color values are in the range 0.0f to 1.0f. Default color is white
	// Default Segment = 36
	//-------------------------------------------------------------------------------------------
	inline void DrawFillCircle(const Vector2& center, const float radius, const int segments = 36, const Color& color = Color())
	{
		if (radius <= 0.0f) return;

		// Iterate through each y-coordinate in the bounding box of the circle
		for (int y = static_cast<int>(center.y - radius); y <= static_cast<int>(center.y + radius); ++y)
		{
			// Calculate the vertical distance from the circle's center
			const float dy = static_cast<float>(y) - center.y;

			// Calculate the horizontal distance using the circle equation: x^2 + y^2 = r^2
			const float dx = std::sqrt(radius * radius - dy * dy);

			// Define the left and right points on this line
			Vector2 left(center.x - dx, static_cast<float>(y));
			Vector2 right(center.x + dx, static_cast<float>(y));

			// Draw a horizontal line to fill the circle
			DrawLine(left, right, color);
		}

		// Optionally, draw the circle outline
		DrawCircle(center, radius, segments, color);
	}

	//-------------------------------------------------------------------------------------------
	// Draw a filled polygon based on the vertices(vector<Vector2>)
	// Color values are in the range 0.0f to 1.0f. Default color is white
	//-------------------------------------------------------------------------------------------
	inline void DrawFillPolygon(const std::vector<Vector2>& vertices, const Color& color)
	{
		if (vertices.size() < 3) return;

		// Calculate the bounding box of the polygon
		float minY = vertices[0].y, maxY = vertices[0].y;
		float minX = vertices[0].x, maxX = vertices[0].x;

		for (const auto& vertex : vertices)
		{
			minY = (((minY) < (vertex.y)) ? (minY) : (vertex.y));
			maxY = (((maxY) > (vertex.y)) ? (maxY) : (vertex.y));
			minX = (((minX) < (vertex.x)) ? (minX) : (vertex.x));
			maxX = (((maxX) > (vertex.x)) ? (maxX) : (vertex.x));
		}

		// Iterate through each y-coordinate in the bounding box
		const int startY = static_cast<int>(minY);
		const int endY = static_cast<int>(maxY);

		std::vector<float> intersections;
		intersections.reserve(vertices.size());

		for (int y = startY; y <= endY; ++y)
		{
			const float yCoordinate = static_cast<float>(y);
			intersections.clear();

			// Find intersections of the polygon's edges with the current y-coordinate
			for (size_t i = 0; i < vertices.size(); ++i)
			{
				const Vector2& v1 = vertices[i];
				const Vector2& v2 = vertices[(i + 1) % vertices.size()];

				// Skip horizontal edges
				if (std::abs(v1.y - v2.y) < FLT_EPSILON) continue;

				// Check if the current y-coordinate intersects the edge
				if ((yCoordinate >= v1.y && yCoordinate <= v2.y) || (yCoordinate >= v2.y && yCoordinate <= v1.y))
				{
					float t = (yCoordinate - v1.y) / (v2.y - v1.y);
					float x = v1.x + t * (v2.x - v1.x);
					intersections.push_back(x);
				}
			}

			// Sort intersections by x-coordinate
			std::sort(intersections.begin(), intersections.end());

			if (intersections.size() >= 2)  // Add this check
			{
				// Draw horizontal lines between pairs of intersections
				for (size_t i = 0; i < intersections.size() - 1; i += 2)
				{
					Vector2 left(intersections[i], yCoordinate);
					Vector2 right(intersections[i + 1], yCoordinate);
					DrawLine(left, right, color);
				}
			}
		}

		// Polygon outline
		DrawPolygon(vertices, color);
	}

	//-------------------------------------------------------------------------------------------
	// Draw a filled rectangle with the specified bottom-left corner, width, height, and color
	// Color values are in the range 0.0f to 1.0f. Default color is white
	//-------------------------------------------------------------------------------------------
	inline void DrawFillRectangle(const Vector2& bottomLeft, const float width, const float height, const Color& color = Color())
	{
		// Iterate through each y-coordinate in the rectangle
		for (int y = static_cast<int>(bottomLeft.y); y <= static_cast<int>(bottomLeft.y + height); ++y)
		{
			Vector2 left(bottomLeft.x, static_cast<float>(y));
			Vector2 right(bottomLeft.x + width, static_cast<float>(y));
			DrawLine(left, right, color);
		}

		// Define the rectangle's vertices
		const std::vector<Vector2> vertices = {
			bottomLeft,
			{bottomLeft.x + width, bottomLeft.y},
			{bottomLeft.x + width, bottomLeft.y + height},
			{bottomLeft.x, bottomLeft.y + height}
		};

		// Rectangle outline
		DrawPolygon(vertices, color);
	}
}