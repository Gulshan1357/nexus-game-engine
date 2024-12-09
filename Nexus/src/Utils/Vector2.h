#pragma once

#include <string>

// TODO: Check this for optimization => https://www.reddit.com/r/cpp_questions/comments/190rmzc/code_review_vector2/
// TODO: Check if Vector2 * Vector2 is useful. If not, change it to cross product

struct Vector2
{
	float x, y;

	Vector2() : x(0.0f), y(0.0f) {}
	Vector2(float newX, float newY) : x(newX), y(newY) {}

	Vector2 operator+(const Vector2& vector) const;
	Vector2 operator-(const Vector2& vector) const;
	Vector2 operator*(float scalar) const;
	Vector2 operator*(const Vector2& vector) const;
	Vector2 operator/(float scalar) const;
	Vector2 operator/(const Vector2& vector) const;

	Vector2& operator-();

	Vector2& operator+=(const Vector2& vector);
	Vector2& operator-=(const Vector2& vector);
	Vector2& operator*=(float scalar);
	Vector2& operator*=(const Vector2& vector);
	Vector2& operator/=(float scalar);
	Vector2& operator/=(const Vector2& vector);

	bool operator==(const Vector2& vector) const;
	bool operator!=(const Vector2& vector) const;

	[[nodiscard]] float Length() const;
	[[nodiscard]] float Distance(const Vector2& to) const;
	[[nodiscard]] Vector2 Normalized() const;
	[[nodiscard]] std::string ToString() const;
};

inline Vector2 Vector2::operator+(const Vector2& vector) const
{
	return { this->x + vector.x, this->y + vector.y };
}

inline Vector2 Vector2::operator-(const Vector2& vector) const
{
	return { this->x - vector.x, this->y - vector.y };
}

inline Vector2 Vector2::operator*(float scalar) const
{
	return { this->x * scalar, this->y * scalar };
}

inline Vector2 Vector2::operator*(const Vector2& vector) const
{
	return { this->x * vector.x, this->y * vector.y };
}

inline Vector2 Vector2::operator/(float scalar) const
{
	return { this->x / scalar, this->y / scalar };
}

inline Vector2 Vector2::operator/(const Vector2& vector) const
{
	return { this->x / vector.x, this->y / vector.y };
}

inline Vector2& Vector2::operator-()
{
	x = -x;
	y = -y;
	return *this;
}

inline Vector2& Vector2::operator+=(const Vector2& vector)
{
	x += vector.x;
	y += vector.y;
	return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& vector)
{
	x -= vector.x;
	y -= vector.y;
	return *this;
}

inline Vector2& Vector2::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}

inline Vector2& Vector2::operator*=(const Vector2& vector)
{
	x *= vector.x;
	y *= vector.y;
	return *this;
}

inline Vector2& Vector2::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	return *this;
}

inline Vector2& Vector2::operator/=(const Vector2& vector)
{
	x /= vector.x;
	y /= vector.y;
	return *this;
}

inline bool Vector2::operator==(const Vector2& vector) const
{
	return x == vector.x && y == vector.y;
}

inline bool Vector2::operator!=(const Vector2& vector) const
{
	return x != vector.x || y != vector.y;
}

inline float Vector2::Length() const
{
	return sqrtf(x * x + y * y);
}

inline float Vector2::Distance(const Vector2& to) const
{
	return abs((to - *this).Length());
}

inline Vector2 Vector2::Normalized() const
{
	Vector2 v;
	if (const float length = Length(); length > 0)
		v = *this / length;
	return v;
}

inline std::string Vector2::ToString() const
{
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}