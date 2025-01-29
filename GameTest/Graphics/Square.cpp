#include "stdafx.h"

#include "../App/app.h"
#include "Square.h"

Square::Square()
{
	points[0] = Vec3(0, 0, 0);
	points[1] = Vec3(0, 0, 0);
	points[2] = Vec3(0, 0, 0);
	points[3] = Vec3(0, 0, 0);

}
Square::Square(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4)
{
	points[0] = p1;
	points[1] = p2;
	points[2] = p3;
	points[3] = p4;
}


void Square::operator *= (const Mat4& rhs)
{
	// Apply transformation to triangle points
	points[0] = rhs * points[0];
	points[1] = rhs * points[1];
	points[2] = rhs * points[2];
	points[3] = rhs * points[3];
}

void Square::transformNormal(const Mat4& rhs)
{
	normals[0] = (rhs * normals[0]).normalize();
	normals[1] = (rhs * normals[1]).normalize();
	normals[2] = (rhs * normals[2]).normalize();
	normals[3] = (rhs * normals[3]).normalize();

}

Vec3 Square::GetNormal() const
{
	Vec3 line1, line2;

	line1 = points[1] - points[0];
	line2 = points[2] - points[0];

	return isCW ? (line1.cross_product(line2)).normalize() : (line2.cross_product(line1)).normalize();
}


void Square::DrawOutline() const
{
	float r = lineColor.x, g = lineColor.y, b = lineColor.z;

	App::DrawLine(points[0].x, points[0].y, points[1].x, points[1].y, r, g, b);
	App::DrawLine(points[1].x, points[1].y, points[2].x, points[2].y, r, g, b);
	App::DrawLine(points[2].x, points[2].y, points[3].x, points[3].y, r, g, b);
	App::DrawLine(points[3].x, points[3].y, points[0].x, points[0].y, r, g, b);
}