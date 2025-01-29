#include "stdafx.h"

#include "../App/app.h"
#include "Triangle.h"
#include "../Math/Vec2.h"

Triangle::Triangle()
{
	points[0] = Vec3(0, 0, 0);
	points[1] = Vec3(0, 0, 0);
	points[2] = Vec3(0, 0, 0);

}
Triangle::Triangle(Vec3 p1, Vec3 p2, Vec3 p3)
{
	points[0] = p1;
	points[1] = p2;
	points[2] = p3;
}


void Triangle::operator *= (const Mat4& rhs)
{
	// Apply transformation to triangle points
	points[0] = rhs * points[0];
	points[1] = rhs * points[1];
	points[2] = rhs * points[2];
}

void Triangle::transformNormal(const Mat4& rhs)
{
	normals[0] = (rhs * normals[0]).normalize();
	normals[1] = (rhs * normals[1]).normalize();
	normals[2] = (rhs * normals[2]).normalize();

}

Vec3 Triangle::GetNormal() const
{
	Vec3 line1, line2;

	line1 = points[1] - points[0];
	line2 = points[2] - points[0];

	return isCW ? (line1.cross_product(line2)).normalize() : (line2.cross_product(line1)).normalize();
}

float Triangle::GetArea() const
{
	return abs((points[0].x * (points[1].y - points[2].y) + points[1].x * (points[2].y - points[0].y) + points[2].x * (points[0].y - points[1].y)) / 2.0);
}

void Triangle::DrawOutline() const
{
	float r = lineColor.x, g = lineColor.y, b = lineColor.z;

	App::DrawLine(points[0].x, points[0].y, points[1].x, points[1].y, r, g, b);
	App::DrawLine(points[1].x, points[1].y, points[2].x, points[2].y, r, g, b);
	App::DrawLine(points[2].x, points[2].y, points[0].x, points[0].y, r, g, b);
}

// Using Pineda’s Rasterization Algorithm to use fill in triangle
// Unlike Scanline algorithm, rasterization can be done parallel
//void Triangle::Fill() const
//{
//	float r = fillColor.x, g = fillColor.y, b = fillColor.z;
//
//	// bounding box for the triangle, all the points in the box are candidates for triangle fill
//	int x_min = floor(min(min(points[0].x, points[1].x), points[2].x));
//	int x_max = floor(max(max(points[0].x, points[1].x), points[2].x));
//	int y_min = floor(min(min(points[0].y, points[1].y), points[2].y));
//	int y_max = floor(max(max(points[0].y, points[1].y), points[2].y));
//
//
//	auto PointInTriangle = [this](Vec2& pixel) -> bool
//		{
//
//			for (int i = 0; i < 3; i++)
//			{
//				int j = (i != 2) ? i + 1 : 0;
//				Vec2 edge(points[i].x - points[j].x, points[i].y - points[j].y);
//				Vec2 lineToPixel(points[i].x - pixel.x, points[i].y - pixel.y);
//
//				if (!isCW && edge.cross_product(lineToPixel) > 0.0f)
//				{
//					return false;
//				}
//
//				else if (isCW && edge.cross_product(lineToPixel) < 0.0f)
//				{
//					return false;
//				}
//			}
//			return true;
//		};
//
//	// need to determine whether each point is in the triangle or not
//	for (int y = y_min; y <= y_max; y++)
//	{
//		for (int x = x_min; x <= x_max; x++)
//		{
//			Vec2 point(x, y);
//			// point is in triangle if all 3 cross product is positive
//			if (PointInTriangle(point))
//			{
//				App::DrawLine(x, y, x + 1, y + 1, r, g, b);
//			}
//		}
//	}
//}

// Using Scanline Filling Algorithm to use fill in triangle
//void Triangle::Fill() const
//{
//	float r = fillColor.x, g = fillColor.y, b = fillColor.z;
//
//	App::DrawLine(points[0].x, points[0].y, points[1].x, points[1].y, r, g, b);
//	App::DrawLine(points[1].x, points[1].y, points[2].x, points[2].y, r, g, b);
//	App::DrawLine(points[2].x, points[2].y, points[0].x, points[0].y, r, g, b);
//
//	Vec2 orderedPoints[3] = {{points[0].x, points[0].y}, {points[1].x, points[1].y}, {points[2].x, points[2].y}};
//	
//	auto swap_elements = [&orderedPoints](int pos1, int pos2)
//		{
//			Vec2 temp = orderedPoints[pos1];
//			orderedPoints[pos1] = orderedPoints[pos2];
//			orderedPoints[pos2] = temp;
//		};
//
//	// Sort the points so that y_0 <= y_1 <= y_2
//	if (orderedPoints[1].y < orderedPoints[0].y) { swap_elements(1, 0); }
//	if (orderedPoints[2].y < orderedPoints[0].y) { swap_elements(2, 0); }
//	if (orderedPoints[2].y < orderedPoints[1].y) { swap_elements(2, 1); }
//
//	// Compute the interpolated points of the edges
//	std::vector<Vec2> side1 = orderedPoints[0].interpolateX(orderedPoints[2]);
//	std::vector<Vec2> side2 = orderedPoints[0].interpolateX(orderedPoints[1]);
//	std::vector<Vec2> side2_more = orderedPoints[1].interpolateX(orderedPoints[2]);
//
//	//// Fill in the middle at orderedPoint 1, with slope from side 1 
//	//float yi = orderedPoints[1].y;
//	//float xi = orderedPoints[1].x + ((yi - orderedPoints[0].y) * (orderedPoints[2].x - orderedPoints[0].x) / (orderedPoints[2].y - orderedPoints[0].y));
//
//	//App::DrawLine(xi, yi, orderedPoints[1].x, orderedPoints[1].y, r, g, b);
//
//	//// Pop the overllap
//	//if (side2.size() != 0 && side2_more.size() != 0)
//	//{
//	//	side2.pop_back();
//	//}
//
//	side2.insert(side2.end(), side2_more.begin(), side2_more.end());
//
//	size_t N = side2.size();
//
//	// need to determine whether each point is in the triangle or not
//	for (int i = 0; i < N; i++)
//	{
//		App::DrawLine(side1[i].x, side1[i].y, side2[i].x, side2[i].y, r, g, b);
//	}
//
//}