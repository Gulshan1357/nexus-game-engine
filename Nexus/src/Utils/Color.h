#pragma once

// Convert the Colors enum into 0.0f to 1.0f RBG values using Color(). Example: Color(Colors::CYAN)
enum struct Colors
{
	BLACK, WHITE, RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE, LIGHT_GREEN, LIGHT_BLUE, PINK, GRAY,
	LIGHT_GRAY, DARK_GRAY, CYAN, TEAL, MAGENTA, BEIGE, MINT, SALMON, LAVENDER, GOLD, NAVY,

	NEON_PINK, NEON_BLUE, NEON_PURPLE, NEON_GREEN, NEON_YELLOW, NEON_ORANGE, NEON_RED,
	NEON_AQUA, NEON_MAGENTA, NEON_LIME, DARK_BLUE, DARK_PURPLE, DARK_CYAN, BG_DARK_BLUE, GRID_BLUE
};

struct Color
{
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;

	Color() = default;

	Color(const float r, const float g, const float b) : r(r), g(g), b(b) {}

	explicit Color(const Colors col)
	{
		switch (col)
		{
			case Colors::BLACK:        r = 0.0f;   g = 0.0f;   b = 0.0f;   break;
			case Colors::WHITE:        r = 1.0f;   g = 1.0f;   b = 1.0f;   break;
			case Colors::RED:          r = 1.0f;   g = 0.0f;   b = 0.0f;   break;
			case Colors::ORANGE:       r = 1.0f;   g = 0.647f; b = 0.0f;   break;
			case Colors::YELLOW:       r = 1.0f;   g = 1.0f;   b = 0.0f;   break;
			case Colors::GREEN:        r = 0.0f;   g = 1.0f;   b = 0.0f;   break;
			case Colors::BLUE:         r = 0.0f;   g = 0.0f;   b = 1.0f;   break;
			case Colors::PURPLE:       r = 0.502f; g = 0.0f;   b = 0.502f; break;
			case Colors::LIGHT_GREEN:  r = 0.564f; g = 0.933f; b = 0.564f; break;
			case Colors::LIGHT_BLUE:   r = 0.678f; g = 0.847f; b = 0.902f; break;
			case Colors::PINK:         r = 1.0f;   g = 0.753f; b = 0.796f; break;
			case Colors::GRAY:         r = 0.502f; g = 0.502f; b = 0.502f; break;
			case Colors::LIGHT_GRAY:   r = 0.827f; g = 0.827f; b = 0.827f; break;
			case Colors::DARK_GRAY:    r = 0.251f; g = 0.251f; b = 0.251f; break;
			case Colors::CYAN:         r = 0.0f;   g = 1.0f;   b = 1.0f;   break;
			case Colors::TEAL:         r = 0.0f;   g = 0.502f; b = 0.502f; break;
			case Colors::MAGENTA:      r = 1.0f;   g = 0.0f;   b = 1.0f;   break;
			case Colors::BEIGE:        r = 0.961f; g = 0.961f; b = 0.863f; break;
			case Colors::MINT:         r = 0.596f; g = 1.0f;   b = 0.596f; break;
			case Colors::SALMON:       r = 0.980f; g = 0.502f; b = 0.447f; break;
			case Colors::LAVENDER:     r = 0.902f; g = 0.902f; b = 0.980f; break;
			case Colors::GOLD:         r = 1.0f;   g = 0.843f; b = 0.0f;   break;
			case Colors::NAVY:         r = 0.0f;   g = 0.0f;   b = 0.502f; break;

			case Colors::NEON_PINK:    r = 1.0f;   g = 0.07f;  b = 0.57f;  break;
			case Colors::NEON_BLUE:    r = 0.0f;   g = 0.91f;  b = 0.96f;  break;
			case Colors::NEON_PURPLE:  r = 0.7f;   g = 0.0f;   b = 1.0f;   break;
			case Colors::NEON_GREEN:   r = 0.0f;   g = 1.0f;   b = 0.4f;   break;
			case Colors::NEON_YELLOW:  r = 1.0f;   g = 0.95f;  b = 0.0f;   break;
			case Colors::NEON_ORANGE:  r = 1.0f;   g = 0.45f;  b = 0.0f;   break;
			case Colors::NEON_RED:     r = 1.0f;   g = 0.0f;   b = 0.2f;   break;
			case Colors::NEON_AQUA:    r = 0.0f;   g = 1.0f;   b = 0.8f;   break;
			case Colors::NEON_MAGENTA: r = 1.0f;   g = 0.0f;   b = 0.8f;   break;
			case Colors::NEON_LIME:    r = 0.6f;   g = 1.0f;   b = 0.0f;   break;
			case Colors::DARK_BLUE:    r = 0.0f;   g = 0.12f;  b = 0.24f;  break;
			case Colors::DARK_PURPLE:  r = 0.15f;  g = 0.0f;   b = 0.2f;   break;
			case Colors::DARK_CYAN:    r = 0.0f;   g = 0.2f;   b = 0.2f;   break;
			case Colors::BG_DARK_BLUE: r = 0.05f;  g = 0.05f;  b = 0.1f;   break;
			case Colors::GRID_BLUE:    r = 0.1f;   g = 0.2f;   b = 0.3f;   break;
		}
	}
};