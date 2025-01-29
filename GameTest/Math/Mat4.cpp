#include "stdafx.h"
#include "Mat4.h"
#include "Vec2.h"
#include "../App/app.h"

#include <iostream>
#include <cmath>
#include <vector>

//const float PI = 3.1415162f;

Mat4::Mat4()
{
}

Mat4::Mat4(float matrixIn[16])
{
	for (int i = 0; i < 16; i++)
	{
		matrix[i] = matrixIn[i];
	}
}

bool Mat4::operator == (const Mat4& rhs) const
{
	// number of columns and rows have to be same
	// since we are only comparing Mat4 with Mat4 this is always true
	for (int i = 0; i < 16; i++)
	{
		// all the elements have to be same
		if (matrix[i] != rhs.matrix[i])
		{
			return false;
		}
	}

	return true;
}
bool Mat4::operator != (const Mat4& rhs) const
{
	for (int i = 0; i < 16; i++)
	{
		// if one of the element is different not same
		if (matrix[i] != rhs.matrix[i])
		{
			return true;
		}
	}
	return false;
}

Mat4 Mat4::operator + (const Mat4& rhs) const
{
	float matrix_sum[16] = { 0 };

	for (int i = 0; i < 16; i++)
	{
		matrix_sum[i] = matrix[i] + rhs.matrix[i];
	}

	return Mat4(matrix_sum);
}

Mat4 Mat4::operator - (const Mat4& rhs) const
{
	float matrix_sum[16] = { 0 };

	for (int i = 0; i < 16; i++)
	{
		matrix_sum[i] = matrix[i] - rhs.matrix[i];
	}

	return Mat4(matrix_sum);
}

Mat4 Mat4::operator * (const Mat4& rhs) const
{
	float matrix_product[16] = { 0 };

	for (int row = 0; row < 4; ++row) // Loop over rows of the left matrix
	{
		for (int col = 0; col < 4; ++col) // Loop over columns of the right matrix
		{
			// Perform dot product of row of lhs with column of rhs
			matrix_product[row * 4 + col] =
				matrix[row * 4 + 0] * rhs.matrix[0 * 4 + col] +
				matrix[row * 4 + 1] * rhs.matrix[1 * 4 + col] +
				matrix[row * 4 + 2] * rhs.matrix[2 * 4 + col] +
				matrix[row * 4 + 3] * rhs.matrix[3 * 4 + col];
		}
	}

	return Mat4(matrix_product);
}


Vec3 Mat4::operator * (const Vec3& rhs) const
{
	float newX = matrix[0] * rhs.x + matrix[1] * rhs.y + matrix[2] * rhs.z + matrix[3] * 1;
	float newY = matrix[4] * rhs.x + matrix[5] * rhs.y + matrix[6] * rhs.z + matrix[7] * 1;
	float newZ = matrix[8] * rhs.x + matrix[9] * rhs.y + matrix[10] * rhs.z + matrix[11] * 1;
	float w = matrix[12] * rhs.x + matrix[13] * rhs.y + matrix[14] * rhs.z + matrix[15] * 1;

	if (w != 0.f)
	{
		newX /= w;
		newY /= w;
		newZ /= w;
	}

	return Vec3(newX, newY, newZ);
}

void Mat4::operator += (const Mat4& rhs)
{
	for (int i = 0; i < 16; i++)
	{
		matrix[i] += rhs.matrix[i];
	}
}

void Mat4::operator -= (const Mat4& rhs)
{
	for (int i = 0; i < 16; i++)
	{
		matrix[i] -= rhs.matrix[i];
	}
}

void Mat4::operator *= (const Mat4& val)
{
	Mat4 product = *(this) * val;

	for (int i = 0; i < 16; i++)
	{
		matrix[i] = product.matrix[i];
	}
}

// 1 0 0 x
// 0 1 0 y
// 0 0 1 z
// 0 0 0 1
void Mat4::Translate(const Vec3& size)
{
	float transform[16] =	{	1, 0, 0, size.x, 
								0, 1, 0, size.y, 
								0, 0, 1, size.z, 
								0, 0, 0, 1 };
	for (int i = 0; i < 16; i++)
	{
		matrix[i] = transform[i];
	}
}

// x 0 0 0
// 0 y 0 0
// 0 0 z 0
// 0 0 0 1
void Mat4::Scale(const Vec3& size)
{
	float transform[16] = {	size.x, 0, 0, 0,
							0, size.y, 0, 0,
							0, 0, size.z, 0,
							0, 0, 0, 1};
	for (int i = 0; i < 16; i++)
	{
		matrix[i] = transform[i];
	}
}

//  1        0         0       0
//  1  cos(theta) -sin(theta)  0
//  1  sin(theta)  cos(theta)  0
//  0        0         0       1
void Mat4::XRotate(float degree)
{
	float theta = degree * (PI / 180);

	float transform[16] = {	1, 0, 0, 0,
							0, cos(theta), -sin(theta), 0,
							0, sin(theta), cos(theta), 0,
							0, 0, 0, 1 };
	for (int i = 0; i < 16; i++)
	{
		matrix[i] = transform[i];
	}
}

//  cos(theta)  0    sin(theta)   0
//     0        1        0        0
// -sin(theta)  0    cos(theta)   0
//     0        0        0        1
void Mat4::YRotate(float degree)
{
	float theta = degree * (PI / 180);

	float transform[16] = { cos(theta), 0, sin(theta), 0,
							0, 1, 0, 0,
							-sin(theta), 0, cos(theta), 0,
							0, 0, 0, 1 };

	for (int i = 0; i < 16; i++)
	{
		matrix[i] = transform[i];
	}
}

//   cos(theta) -sin(theta) 0       0
//   sin(theta)  cos(theta) 0       0
//      0         0         1       0
//      0         0         0       1
void Mat4::ZRotate(float degree)
{
	float theta = degree * (PI / 180);

	float transform[16] = { cos(theta), -sin(theta), 0, 0,
							sin(theta), cos(theta), 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1 };

	for (int i = 0; i < 16; i++)
	{
		matrix[i] = transform[i];
	}
}

// 1 0 0 0
// 0 1 0 0
// 0 0 1 0
// 0 0 0 1
void Mat4::Identity() {
	float transform[16] = { 1, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1 };

	for (int i = 0; i < 16; i++)
	{
		matrix[i] = transform[i];
	}
}

// Function to transpose the matrix
Mat4 Mat4::Transpose() const {
	Mat4 temp;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp.matrix[i * 4 + j] = matrix[j * 4 + i];
		}
	}
	return temp;
}

// Function to compute the inverse of the matrix
Mat4 Mat4::Inverse() const {
	Mat4 temp;
	float det = 0.0f;

	// Calculate the determinant using Laplace expansion
	for (int i = 0; i < 4; i++) {
		det += matrix[i] * (
			matrix[4 + ((i + 1) % 4)] * (matrix[8 + ((i + 2) % 4)] * matrix[12 + ((i + 3) % 4)] -
				matrix[12 + ((i + 2) % 4)] * matrix[8 + ((i + 3) % 4)]) -
			matrix[8 + ((i + 1) % 4)] * (matrix[4 + ((i + 2) % 4)] * matrix[12 + ((i + 3) % 4)] -
				matrix[12 + ((i + 2) % 4)] * matrix[4 + ((i + 3) % 4)]) +
			matrix[12 + ((i + 1) % 4)] * (matrix[4 + ((i + 2) % 4)] * matrix[8 + ((i + 3) % 4)] -
				matrix[8 + ((i + 2) % 4)] * matrix[4 + ((i + 3) % 4)]));
	}

	if (det == 0.0f) {
		throw std::runtime_error("Matrix is singular and cannot be inverted.");
	}

	det = 1.0f / det;

	// Compute the adjugate and divide by determinant
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp.matrix[i * 4 + j] = det * (
				matrix[((j + 1) % 4) * 4 + ((i + 1) % 4)] * (matrix[((j + 2) % 4) * 4 + ((i + 2) % 4)] * matrix[((j + 3) % 4) * 4 + ((i + 3) % 4)] -
					matrix[((j + 3) % 4) * 4 + ((i + 2) % 4)] * matrix[((j + 2) % 4) * 4 + ((i + 3) % 4)]) -
				matrix[((j + 2) % 4) * 4 + ((i + 1) % 4)] * (matrix[((j + 1) % 4) * 4 + ((i + 2) % 4)] * matrix[((j + 3) % 4) * 4 + ((i + 3) % 4)] -
					matrix[((j + 3) % 4) * 4 + ((i + 2) % 4)] * matrix[((j + 1) % 4) * 4 + ((i + 3) % 4)]) +
				matrix[((j + 3) % 4) * 4 + ((i + 1) % 4)] * (matrix[((j + 1) % 4) * 4 + ((i + 2) % 4)] * matrix[((j + 2) % 4) * 4 + ((i + 3) % 4)] -
					matrix[((j + 2) % 4) * 4 + ((i + 2) % 4)] * matrix[((j + 1) % 4) * 4 + ((i + 3) % 4)]));
		}
	}

	return temp;
}

// d, 0, 0, 0,
// 0, d, 0, 0,
// 0, 0, -A, -B,
// 0, 0, -1, 0
// where A = -(f+n)/(f-n) and B = -2fn/(f-n)
// A and B are pseudo-distance which ensures lines to map to lines
// preserving the relative distance

void Mat4::Perspective(float r, float t, float n, float f)
{
	// In this case, we let d, the distance from origin to viewing place be the near clip plane
	// And as -n = , we can simplify:
	float transform[16] = { n / r, 0, 0, 0,
							0, n / t, 0, 0,
							0, 0, -(f + n) / (f - n), -(2 * f * n) / (f - n),
							0, 0, -1, 0 };


	for (int i = 0; i < 16; i++)
	{
		matrix[i] = transform[i];
	}
}


//	2 / (r - l)		0			0		-(r + l) / (r - l)
//		0		2 / (t - b)		0		-(t + b) / (t - b)
//		0			0		-2 / (f - n)  -(f + n) / (f - n)
//		0			0			0				 1
// Note: The signs of third row can be reversed depending on what z represent
// In openGL, z =−1 for the closest objects and z = 1 for the farthest objects
void Mat4::Orthographic(float l, float r, float b, float t, float n, float f)
{
	// in canonical view volume,	l = -1 and r = +1,  x axis
	//								b = -1 and t = +1,	y axis
	//								f = +1 and n = -1	z axis
	float transform[16] = { 2 / (r - l), 0, 0, -(r + l) / (r - l),
							0, 2 / (t - b), 0, -(t + b) / (t - b),
							0, 0, -2 / (f - n), -(f + n) / (f - n),
							0, 0, 0, 1 };


	// simplified, assuming view volume is symmetrical
	//float transform[16] = { 1/ r, 0, 0, 0,
	//						0, 1 / t, 0, 0,
	//						0, 0, -2 / (f - n), -(f + n) / (f - n),
	//						0, 0, 0, 1 };

	for (int i = 0; i < 16; i++)
	{
		matrix[i] = transform[i];

	}
}


// Projection is orthographic * perspective
void Mat4::Projection(float fovAngle, float screenWidth, float screenHeight, float n, float f)
{
	Mat4 pers, orth;

	float theta = fovAngle * (PI / 180.0f);
	float aspectRatio = screenWidth / screenHeight;

	// Assume frustrum is centered on the z-axis (when z = 0)
	// Let length of r and l, t and b be equal length
	// then, r = -l and t = -b
	// If angle of view is theta, the right triangle formed on axis have theta/2 
	// then b = -n*tan(theta/2) and r = n*(aspect_ratio)*tan(theta/2)
	float t = n * tan(theta / 2);
	float b = -t;
	float r = aspectRatio * t;
	float l = -r;

	pers.Perspective(r, t, n, f);
	orth.Orthographic(l, r, b, t, n, f);

	// Apply perspective transformation first - takes frustrum view volume to orthographic view volume
	// Apply orthographic orthographic next - aligns the orthographic view volume to the center and scales to normalized form
	Mat4 proj = orth * pers;



	for (int i = 0; i < 16; i++)
	{
		matrix[i] = proj.matrix[i];
		//matrix[i] = transform[i];

	}
};

void Mat4::Viewport(float screenLeft, float screenBottom, float screenWidth, float screenHeight, float n, float f)
{
	float transform[16] = { screenWidth / 2, 0, 0, screenLeft + (screenWidth / 2),
							0, screenHeight / 2, 0, screenBottom + (screenHeight / 2),
							0, 0, (f - n) / 2, (f + n) / 2,
							0, 0, 0, 1 };

	for (int i = 0; i < 16; i++)
	{
		matrix[i] = transform[i];

	}

}

// moves the
void Mat4::PointAt(Vec3 const& camPos, Vec3 const& objPos, Vec3 const& worldUp)
{
	// Calculate normalized direction vector from the camera position to object position
	Vec3 zAxis = camPos - objPos;
	zAxis = zAxis.normalize();

	// Need to generate two orthgonal vectors to new direction vector to define orthogonal basis
	// 1. Need to find new up vector that adjusts to the new direction vector
	Vec3 xAxis = worldUp.cross_product(zAxis);
	xAxis = xAxis.normalize();

	// 2. Create new the new side vector by taking the cross product of the two
	Vec3 yAxis = zAxis.cross_product(xAxis);
	yAxis = yAxis.normalize();

	// translate to the origin, then rotate so that vector from eye lines up
	float transform[16] = { xAxis.x, xAxis.y, xAxis.z, -(camPos.dot_product(xAxis)),
							yAxis.x, yAxis.y, yAxis.z, -(camPos.dot_product(yAxis)),
							zAxis.x, zAxis.y, zAxis.z, -(camPos.dot_product(zAxis)),
							0, 0, 0, 1 };

	for (int i = 0; i < 16; i++)
	{
		matrix[i] = transform[i];

	}

}

// Inverse of PointAt Matrix, takes the object and transforms
void Mat4::LookAt(Vec3 const& camPos, Vec3 const& objPos, Vec3 const& worldUp)
{
	// Calculate normalized direction vector from the camera position to object position
	Vec3 zAxis = camPos - objPos;
	zAxis = zAxis.normalize();

	// Need to generate two orthgonal vectors to new direction vector to define orthogonal basis
	// 1. Need to find new up vector that adjusts to the new direction vector
	Vec3 xAxis = worldUp.cross_product(zAxis);
	xAxis = xAxis.normalize();

	// 2. Create new the new side vector by taking the cross product of the two
	Vec3 yAxis = zAxis.cross_product(xAxis);
	yAxis = yAxis.normalize();

	// translate to the origin, then rotate so that vector from eye lines up
	float transform[16] = { xAxis.x, yAxis.x, zAxis.x, 0,
							xAxis.y, yAxis.y, zAxis.y, 0,
							xAxis.z, yAxis.z, zAxis.z, 0,
							-camPos.dot_product(xAxis), -camPos.dot_product(yAxis), -camPos.dot_product(zAxis), 1};

	for (int i = 0; i < 16; i++)
	{
		matrix[i] = transform[i];

	}

	//Mat4 translate, rotate;
	//translate.Identity();
	//translate.matrix[12] = -camPos.x;
	//translate.matrix[13] = -camPos.y;
	//translate.matrix[14] = -camPos.z;

	//rotate.Identity();
	//rotate.matrix[0] = xAxis.x;
	//rotate.matrix[4] = xAxis.y;
	//rotate.matrix[8] = xAxis.z;

	//rotate.matrix[1] = yAxis.x;
	//rotate.matrix[5] = yAxis.y;
	//rotate.matrix[9] = yAxis.z;

	//rotate.matrix[2] = zAxis.x;
	//rotate.matrix[6] = zAxis.y;
	//rotate.matrix[10] = zAxis.z;

	//Mat4 look = rotate * translate;

	//for (int i = 0; i < 16; i++)
	//{
	//	matrix[i] = look.matrix[i];
	//	//matrix[i] = transform[i];

	//}
}