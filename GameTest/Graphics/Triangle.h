#pragma once

#include "../Math/Vec3.h"
#include "../Math/Mat4.h"

class Triangle
{
public:
	Triangle();
	// Points need to be in clockwise rotation, as the normals are calculated assuming so
	Triangle(Vec3 p1, Vec3 p2, Vec3 p3);

	void operator *= (const Mat4& rhs);
	void transformNormal(const Mat4& rhs);
	
	Vec3 GetNormal() const;
	float GetArea() const;

	void DrawOutline() const;
	//void Fill() const;

	// Size is of the normals and points are always same, so no need to be dynamic
	// Using array is more efficient, and stored in stack
	Vec3 points[3];
	Vec3 normals[3];
	Vec3 fillColor = { 1.f, 1.f, 1.f };
	Vec3 lineColor = { 1.f, 1.f, 1.f };

	bool isCW = true;

};