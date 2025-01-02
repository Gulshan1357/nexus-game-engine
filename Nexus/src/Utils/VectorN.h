#pragma once
#include <stdexcept>

struct VectorN
{
	size_t dimensions;
	float* data;

	VectorN();

	explicit VectorN(size_t dimensions);
	VectorN(const VectorN& vector);

	~VectorN();

	VectorN& operator=(const VectorN& vector);				// v1 = v2
	VectorN operator+(const VectorN& vector) const;			// v1 + v2
	VectorN operator-(const VectorN& vector) const;			// v1 - v2
	VectorN operator*(float scalar) const;					// v1 * scalar

	VectorN& operator+=(const VectorN& vector);				// v1 += v2
	VectorN& operator-=(const VectorN& vector);				// v1 -= v2
	VectorN& operator*=(float scalar);						// v *= scalar

	float operator[] (const size_t index) const;			// vector[index] to fetch a value at index.
	float& operator[] (const size_t index);					// vector[index] to assign a value at index.

	[[nodiscard]] float Dot(const VectorN& vector) const;	// Return a dot product. A vector's projection onto another 
	void Zero();											// Put zero everywhere inside the vector 
};


inline VectorN::VectorN() : dimensions(0), data(nullptr) {}

inline VectorN::VectorN(const size_t dimensions) : dimensions(dimensions)
{
	data = new float[dimensions];
}

inline VectorN::VectorN(const VectorN& vector) : dimensions(vector.dimensions)
{
	data = new float[dimensions];
	for (size_t i = 0; i < dimensions; i++)
		data[i] = vector.data[i];
}

inline VectorN::~VectorN()
{
	delete[] data;
}

inline VectorN& VectorN::operator=(const VectorN& vector)
{
	if (this == &vector) return *this; // Handle self-assignment.

	delete[] data;
	dimensions = vector.dimensions;
	data = new float[dimensions];
	for (size_t i = 0; i < dimensions; i++)
		data[i] = vector.data[i];
	return *this;
}

inline VectorN VectorN::operator+(const VectorN& vector) const
{
	VectorN result(dimensions);
	for (size_t i = 0; i < dimensions; i++)
		result.data[i] = data[i] + vector.data[i];
	return result;
}

inline VectorN VectorN::operator-(const VectorN& vector) const
{
	VectorN result(dimensions);
	for (size_t i = 0; i < dimensions; i++)
		result.data[i] = data[i] - vector.data[i];
	return result;
}

inline VectorN VectorN::operator*(float scalar) const
{
	VectorN result(dimensions);
	for (size_t i = 0; i < dimensions; i++)
		result.data[i] = data[i] * scalar;
	return result;
}

inline VectorN& VectorN::operator+=(const VectorN& vector)
{
	for (size_t i = 0; i < dimensions; i++)
		data[i] += vector.data[i];
	return *this;
}

inline VectorN& VectorN::operator-=(const VectorN& vector)
{
	for (size_t i = 0; i < dimensions; i++)
		data[i] -= vector.data[i];
	return *this;
}

inline VectorN& VectorN::operator*=(float scalar)
{
	for (size_t i = 0; i < dimensions; i++)
		data[i] *= scalar;
	return *this;
}

inline float VectorN::operator[](const size_t index) const
{
	return data[index];
}

inline float& VectorN::operator[](const size_t index)
{
	return data[index];
}

inline float VectorN::Dot(const VectorN& vector) const
{
	float sum = 0.0f;
	for (size_t i = 0; i < dimensions; i++)
		sum += data[i] * vector.data[i];
	return sum;
}

inline void VectorN::Zero()
{
	for (size_t i = 0; i < dimensions; i++)
		data[i] = 0.0f;
}
