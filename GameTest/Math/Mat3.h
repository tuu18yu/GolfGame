#pragma once
#include "Vec2.h"

class Mat3
{
public:
	Mat3();
	Mat3(float matrixIn[9]);

	float matrix[9] = { 0 };

	bool operator == (const Mat3& rhs) const;
	bool operator != (const Mat3& rhs) const;

	Mat3 operator + (const Mat3& rhs) const;
	Mat3 operator - (const Mat3& rhs) const;

	// the only matrix multiplication needed for this program is 3x3 x 3x3 and 3x3 x 3x1
	Mat3 operator * (const Mat3& rhs) const;
	// 3x3 matrix should be multiplie to vector of size 3 but only for this program, we are assuming
	// we are only using matrix multplication for homogeous coord in 2D so z is always 1
	Vec2 operator * (const Vec2& rhs) const;

	void operator += (const Mat3& rhs);
	void operator -= (const Mat3& rhs);
	void operator *= (const Mat3& val);


	// Modifies the matrix Affine transformation needed in 2D
	void Translate(const Vec2& size);
	void Scale(const Vec2& size);
	void Rotate(float degree); // clockwise rotation
};
