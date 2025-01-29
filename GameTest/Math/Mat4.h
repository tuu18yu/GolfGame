#pragma once

#include "Vec3.h"

class Mat4
{
public:
	Mat4();
	Mat4(float matrixIn[16]);

	float matrix[16] = { 0 };

	bool operator == (const Mat4& rhs) const;
	bool operator != (const Mat4& rhs) const;

	Mat4 operator + (const Mat4& rhs) const;
	Mat4 operator - (const Mat4& rhs) const;

	// the only matrix multiplication needed for this program is 4x4 x 4x4 and 4x4 x 4x1
	Mat4 operator * (const Mat4& rhs) const;
	// 4x4 matrix should be multiplie to vector of size 4 but only for this program, we are assuming
	// we are only using matrix multplication for homogeous coord in 3D so last elemeny is always 1
	Vec3 operator * (const Vec3& rhs) const;

	void operator += (const Mat4& rhs);
	void operator -= (const Mat4& rhs);
	void operator *= (const Mat4& val);


	// Modifies the matrix to Affine transformation matrix needed in 3D
	void Translate(const Vec3& size);
	void Scale(const Vec3& size);

	// Unlike 2D, rotation can happen on any axis which can be 
	// represented by combination of rotation on x, y, z axes
	// Note: clockwise rotation 
	void XRotate(float degree);
	void YRotate(float degree);
	void ZRotate(float degree);

	void Identity();
	Mat4 Transpose() const;
	Mat4 Inverse() const;

	// Modifies the matrix to Projection Transformation matrix
	//farthest view, nearest view
	void Perspective(float r, float t, float n, float f);
	void Orthographic(float l, float r, float b, float t, float n, float f); // right left top bottom far near
	//void DrawFrustum(float fovAngle, float screenWidth, float screenHeight, float n, float f);
	

	// Projection is orthographic * perspective
	
	// Limits the perspective transformation for which the z-axis is in the
	// center of the field of view
	// angle of field of view, aspect ratio is w/h, nearest view, farthest view
	void Projection(float fovAngle, float screenWidth, float screenHeight, float n, float f);

	// Viewport transform converts the normalized device coordinates (NDC) to window screen coordinates
	void Viewport(float screenLeft, float screenBottom, float screenWidth, float screenHeight, float n, float f);

	// position of camera, direction camera is looking at, the direction of camera looking up
	void PointAt(Vec3 const& camPos, Vec3 const& objPos, Vec3 const& worldUp);
	// 
	void LookAt(Vec3 const& camPos, Vec3 const& objPos, Vec3 const& worldUp);
};
