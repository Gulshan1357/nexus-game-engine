#pragma once

#include "VectorN.h"

struct Matrix
{
	size_t numRows;			// Number of rows in the matrix
	size_t numCols;			// Number of columns in the matrix

	VectorN* rowVectors;	// Array of vectors representing each row of the matrix

	Matrix();
	Matrix(size_t numRows, size_t numCols);
	Matrix(const Matrix& matrix);

	~Matrix();

	Matrix& operator=(const Matrix& matrix) noexcept;	// m1 = m2
	VectorN operator*(const VectorN& vectorN) const;	// matrix * vector
	Matrix operator*(const Matrix& matrix) const;		// m1 * m2

	void Zero();
	[[nodiscard]] Matrix Transpose() const;
};

inline Matrix::Matrix() : numRows(0), numCols(0), rowVectors(nullptr) {}

inline Matrix::Matrix(const size_t numRows, const size_t numCols)
	: numRows(numRows), numCols(numCols), rowVectors(numRows > 0 ? new VectorN[numRows] : nullptr)
{
	for (size_t i = 0; i < numRows; i++)
		rowVectors[i] = VectorN(numCols);
}

inline Matrix::Matrix(const Matrix& matrix) : numRows(0), numCols(0), rowVectors(nullptr)
{
	*this = matrix;
}

inline Matrix::~Matrix()
{
	delete[] rowVectors;
	rowVectors = nullptr;
}

inline Matrix& Matrix::operator=(const Matrix& matrix)
{
	if (this != &matrix) // Prevent self-assignment
	{
		delete[] rowVectors;

		numRows = matrix.numRows;
		numCols = matrix.numCols;

		rowVectors = numRows > 0 ? new VectorN[numRows] : nullptr;
		for (size_t i = 0; i < numRows; i++)
			rowVectors[i] = matrix.rowVectors[i];
	}
	return *this;
}

inline VectorN Matrix::operator*(const VectorN& vectorN) const
{
	if (vectorN.dimensions != numCols)
		throw std::invalid_argument("Matrix and vector dimensions do not match.");
	VectorN result(numRows);
	for (size_t i = 0; i < numRows; i++)
		result[i] = vectorN.Dot(rowVectors[i]);
	return result;
}

inline Matrix Matrix::operator*(const Matrix& matrix) const
{
	if (numCols != matrix.numRows)
		throw std::invalid_argument("Matrix dimensions do not allow multiplication.");
	const Matrix transposed = matrix.Transpose();
	Matrix result(numRows, matrix.numCols);
	for (size_t i = 0; i < numRows; i++)
		for (size_t j = 0; j < matrix.numCols; j++)
			result.rowVectors[i][j] = rowVectors[i].Dot(transposed.rowVectors[j]);
	return result;
}

inline void Matrix::Zero()
{
	for (size_t i = 0; i < numRows; i++)
		rowVectors[i].Zero();
}

inline Matrix Matrix::Transpose() const
{
	Matrix result(numCols, numRows);
	for (size_t i = 0; i < numRows; i++)
		for (size_t j = 0; j < numCols; j++)
			result.rowVectors[j][i] = rowVectors[i][j];
	return result;
}
