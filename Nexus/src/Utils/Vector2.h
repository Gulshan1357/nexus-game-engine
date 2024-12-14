#pragma once

#include <string>

// TODO: Check this for optimization => https://www.reddit.com/r/cpp_questions/comments/190rmzc/code_review_vector2/

struct Vector2
{
	float x, y;

	Vector2() : x(0.0f), y(0.0f) {}
	Vector2(const float newX, const float newY) : x(newX), y(newY) {}

	Vector2 operator+(const Vector2& vector) const;			// v1 + v2
	Vector2 operator-(const Vector2& vector) const;			// v1 - v2
	Vector2 operator*(float scalar) const;					// v1 * scalar
	Vector2 operator*(const Vector2& vector) const;			// (v1.x * v2.x, v1.y * v2.y) 
	Vector2 operator/(float scalar) const;					// v1 / scalar
	Vector2 operator/(const Vector2& vector) const;			// (v1.x / v2.x, v1.y / v2.y)

	Vector2& operator-();									// -v

	Vector2& operator+=(const Vector2& vector);				// v1 += v2
	Vector2& operator-=(const Vector2& vector);				// v1 -= v2
	Vector2& operator*=(float scalar);						// v *= scalar
	Vector2& operator*=(const Vector2& vector);				// v1 *= v2
	Vector2& operator/=(float scalar);						// v1 /= scalar
	Vector2& operator/=(const Vector2& vector);				// v1 /= v2

	bool operator==(const Vector2& vector) const;			// Return true if v1.x == v2.x && v1.y == v2.y
	bool operator!=(const Vector2& vector) const;			// Return true if v1.x != v2.x || v1.y != v2.y

	[[nodiscard]] Vector2 Rotate(const float radians) const;// Returns new vector after rotating original by radians

	[[nodiscard]] float Magnitude() const;					// Returns square root of (x*x + y*y). Use MagnitudeSquared() is possible since it is faster
	[[nodiscard]] float MagnitudeSquared() const;			// Returns (x*x + y*y)
	[[nodiscard]] float Distance(const Vector2& to) const;	// Returns distance between two vector. PointA.Distance(PointB)

	[[nodiscard]] Vector2 UnitVector() const;				// Return a new vector.Normalize() unit vector
	[[nodiscard]] Vector2& Normalize();						// Turn the vector into a unit vector
	[[nodiscard]] Vector2 Normal() const;					// Return a perpendicular vector. x becomes y and y becomes x

	[[nodiscard]] float Dot(const Vector2& vector) const;	// Return a dot product. A vectors projection onto another 
	[[nodiscard]] float Cross(const Vector2& vector) const;	// Return the magnitude of a perpendicular imaginary z-axis vector that is pointing inside/outside the screen. 

	[[nodiscard]] std::string ToString() const;				// Convert the x and v into a nice printable string
};

inline Vector2 Vector2::operator+(const Vector2& vector) const
{
	return { this->x + vector.x, this->y + vector.y };
}

inline Vector2 Vector2::operator-(const Vector2& vector) const
{
	return { this->x - vector.x, this->y - vector.y };
}

inline Vector2 Vector2::operator*(const float scalar) const
{
	return { this->x * scalar, this->y * scalar };
}

inline Vector2 Vector2::operator*(const Vector2& vector) const
{
	return { this->x * vector.x, this->y * vector.y };
}

inline Vector2 Vector2::operator/(const float scalar) const
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

inline Vector2& Vector2::operator*=(const float scalar)
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

inline Vector2& Vector2::operator/=(const float scalar)
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
	constexpr float epsilon = 1e-6f;
	return std::abs(x - vector.x) < epsilon && std::abs(y - vector.y) < epsilon;
	// return x == vector.x && y == vector.y;
}

inline bool Vector2::operator!=(const Vector2& vector) const
{
	return !(*this == vector);
}

inline Vector2 Vector2::Rotate(const float radians) const
{
	return Vector2(x * std::cos(radians) - y * std::sin(radians), x * std::sin(radians) + y * std::cos(radians));
}

inline float Vector2::Magnitude() const
{
	return sqrtf(x * x + y * y);
}

inline float Vector2::MagnitudeSquared() const
{
	return x * x + y * y;
}

inline float Vector2::Distance(const Vector2& to) const
{
	return (to - *this).Magnitude();
}

inline Vector2 Vector2::UnitVector() const
{
	Vector2 v;
	if (const float length = Magnitude(); length != 0)
		v = *this / length;
	return v;
}

inline Vector2& Vector2::Normalize()
{
	if (const float length = Magnitude(); length != 0.0)
	{
		*this /= length;
	}
	return *this;
}

inline Vector2 Vector2::Normal() const
{
	return Vector2(y, -x).Normalize();
}

inline float Vector2::Dot(const Vector2& vector) const
{
	return (x * vector.x) + (y * vector.y);
}

inline float Vector2::Cross(const Vector2& vector) const
{
	return (x * vector.y) - (y * vector.x);
}

inline std::string Vector2::ToString() const
{
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}
