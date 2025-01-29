#include "stdafx.h"
#include "Mat3.h"

#include <iostream>
#include <cmath>

const float PI = 3.141592f;

Mat3::Mat3()
{}

Mat3::Mat3(float matrixIn[9])
{
	for (int i = 0; i < 9; i++)
	{
		matrix[i] = matrixIn[i];
	}
}

bool Mat3::operator == (const Mat3& rhs) const
{
	// number of columns and rows have to be same
	// since we are only comparing Mat3 with Mat3 this is always true
	for (int i = 0; i < 9; i++)
	{
		// all the elements have to be same
		if (matrix[i] != rhs.matrix[i])
		{
			return false;
		}
	}

	return true;
}
bool Mat3::operator != (const Mat3& rhs) const
{
	for (int i = 0; i < 9; i++)
	{
		// if one of the element is different not same
		if (matrix[i] != rhs.matrix[i])
		{
			return true;
		}
	}
	return false;
}

Mat3 Mat3::operator + (const Mat3& rhs) const
{
	float matrix_sum[9] = { 0 };

	for (int i = 0; i < 9; i++)
	{
		matrix_sum[i] = matrix[i] + rhs.matrix[i];
	}

	return Mat3(matrix_sum);
}

Mat3 Mat3::operator - (const Mat3& rhs) const
{
	float matrix_sum[9] = { 0 };

	for (int i = 0; i < 9; i++)
	{
		matrix_sum[i] = matrix[i] - rhs.matrix[i];
	}

	return Mat3(matrix_sum);
}

Mat3 Mat3::operator * (const Mat3& rhs) const
{
	float matrix_product[9] = { 0 };

	for (int i = 0; i < 9; i++)
	{
		float row[3] = { 0 };
		float col[3] = { 0 };

		// row 1 - first row of left matrix
		if (i < 3)
		{
			row[0] = matrix[0];
			row[1] = matrix[1];
			row[2] = matrix[2];
		}
		// row 2 - second row of left matrix
		else if (i > 2 && i < 6)
		{
			row[0] = matrix[3];
			row[1] = matrix[4];
			row[2] = matrix[5];

		}
		// row 3 - third row of left matrix
		else
		{
			row[0] = matrix[6];
			row[1] = matrix[7];
			row[2] = matrix[8];
		}

		// col 1 - first col of right matrix
		if (i == 0 || i == 3 || i == 6)
		{
			col[0] = rhs.matrix[0];
			col[1] = rhs.matrix[3];
			col[2] = rhs.matrix[6];
		}
		// col 2 - second col of right matrix
		else if (i == 1 || i == 4 || i == 7)
		{
			col[0] = rhs.matrix[1];
			col[1] = rhs.matrix[4];
			col[2] = rhs.matrix[7];
		}
		// col 3 - third col of right matrix
		else
		{
			col[0] = rhs.matrix[2];
			col[1] = rhs.matrix[5];
			col[2] = rhs.matrix[8];
		}

		matrix_product[i] = row[0]*col[0] + row[1] * col[1] + row[2] * col[2];
	}

	return Mat3(matrix_product);
}
// assuming last row is 1, returning Vec2 instead of Vec3
//  a b c     i
//  d e f  *  j
//  e f g     1 
Vec2 Mat3::operator * (const Vec2& rhs) const
{
	float newX = matrix[0] * rhs.x + matrix[1] * rhs.y + matrix[2] * 1;
	float newY = matrix[3] * rhs.x + matrix[4] * rhs.y + matrix[5] * 1;
	float z = matrix[6] * rhs.x + matrix[7] * rhs.y + matrix[8] * 1;

	if (z != 0.0f)
	{
		newX /= z;
		newY /= z;
	}

	return Vec2(newX, newY);
}

void Mat3::operator += (const Mat3& rhs)
{
	for (int i = 0; i < 9; i++)
	{
		matrix[i] += rhs.matrix[i];
	}
}

void Mat3::operator -= (const Mat3& rhs)
{
	for (int i = 0; i < 9; i++)
	{
		matrix[i] -= rhs.matrix[i];
	}
}

void Mat3::operator *= (const Mat3& val)
{
	Mat3 product = *(this) * val;

	for (int i = 0; i < 9; i++)
	{
		matrix[i] = product.matrix[i];
	}
}

// 1 0 x
// 0 1 y
// 0 0 1
void Mat3::Translate(const Vec2& size)
{
	float transform[9] = { 1, 0, size.x, 0, 1, size.y, 0, 0, 1 };
	for (int i = 0; i < 9; i++)
	{
		matrix[i] = transform[i];
	}
}

// x 0 0
// 0 y 0
// 0 0 1
void Mat3::Scale(const Vec2& size)
{
	float transform[9] = { size.x, 0, 0, 0, size.y, 0, 0, 0, 1 };
	for (int i = 0; i < 9; i++)
	{
		matrix[i] = transform[i];
	}
}

//  cos(theta) sin(theta) 0
// -sin(theta) cos(theta) 0
//      0          0      1
void Mat3::Rotate(float degree)
{
	// 1 rad = 180 degree / pi
	// 1 degree = pi rad / 180

	// degrees -> radians
	float theta = degree * (PI / 180);

	float transform[9] = { cos(theta), sin(theta), 0, -sin(theta), cos(theta), 0, 0, 0, 1 };
	for (int i = 0; i < 9; i++)
	{
		matrix[i] = transform[i];
	}
}