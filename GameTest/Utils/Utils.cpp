#include "stdafx.h"
#include "Utils.h"

// Equation of plane = xNx + yNy + zNz - (N dot P) = 0
// where P is point on the plane, and N is the normal vector of the plane
// As we are using normal to figure out the slope, dy/dx = -Nx/Ny
Vec3 VectorIntersectPlane(Vec3 const& P_point, Vec3 const& P_normal, Vec3 const& lineStart, Vec3 const& lineEnd)
{
	Vec3 N = P_normal.normalize(); // ensure normal is normalized
	float NdotP = -(N.dot_product(P_point));

	float ad = lineStart.dot_product(N);
	float bd = lineEnd.dot_product(N);
	float t = (-NdotP - ad) / (bd - ad);
	Vec3 lineStartToEnd = lineEnd - lineStart;
	Vec3 lineToIntersect = lineStartToEnd * t;

	return lineStart + lineToIntersect;
}

int TriangleClipAgainstPlane(Vec3 const& P_point, Vec3 const& P_normal, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2)
{
	Vec3 N = P_normal.normalize(); // ensure normal is normalized

	// Return shortest distance from point to plane
	auto dist = [&](Vec3 const & p)
		{
			float NdotP = N.dot_product(P_point);
			Vec3 P = p.normalize();
			return ((N.x * P.x + N.y * P.y + N.z * P.z) - NdotP);
		};

	// Create temp storage to decide whether the points of either side of plane in inside the plane
	Vec3* inside_points[3] = { {}, {}, {} }; int insideCounts = 0;
	Vec3* outside_points[3] = { {}, {}, {} }; int outsideCounts = 0;

	// Get distance of each point of the triangle
	float d0 = dist(in_tri.points[0]);
	float d1 = dist(in_tri.points[1]);
	float d2 = dist(in_tri.points[2]);

	if (d0 >= 0)
	{
		inside_points[insideCounts] = &(in_tri.points[0]);
		insideCounts++;
	}
	else
	{
		outside_points[outsideCounts] = &(in_tri.points[0]);
		outsideCounts++;
	}

	if (d1 >= 0)
	{
		inside_points[insideCounts] = &(in_tri.points[1]);
		insideCounts++;
	}
	else
	{
		outside_points[outsideCounts] = &(in_tri.points[1]);
		outsideCounts++;
	}

	if (d2 >= 0)
	{
		inside_points[insideCounts] = &(in_tri.points[2]);
		insideCounts++;
	}
	else
	{
		outside_points[outsideCounts] = &(in_tri.points[2]);
		outsideCounts++;
	}

	// Classify

	// No points are inside the plane, so ignore the triangle
	if (insideCounts == 0)
	{
		return 0; // no output triangle
	}
	// If all points of triangle is inside the plane, no new triangles need to be form return itself
	else if (insideCounts == 3)
	{
		out_tri1 = in_tri;
		return 1; // 1 output triangle which is just its input triangle
	}
	// If 1 point is inside the plane, we need to form 1 new triangle with the two intersections
	else if (insideCounts == 1)
	{
		 // Need to use the one point inside the plane
		out_tri1.fillColor = in_tri.fillColor;
		out_tri1.lineColor = in_tri.lineColor;

		out_tri1.points[0] = *inside_points[0];
		out_tri1.points[1] = VectorIntersectPlane(P_point, P_normal, *inside_points[0], *outside_points[0]);
		out_tri1.points[2] = VectorIntersectPlane(P_point, P_normal, *inside_points[0], *outside_points[1]);
		return 1;
	}
	// Need to create 2 new triangle
	else if (insideCounts == 2)
	{
		out_tri1.fillColor = in_tri.fillColor;
		out_tri1.lineColor = in_tri.lineColor;

		out_tri1.points[0] = *inside_points[0];
		out_tri1.points[1] = *inside_points[1];
		out_tri1.points[2] = VectorIntersectPlane(P_point, P_normal, *inside_points[0], *outside_points[0]);

		out_tri2.fillColor = in_tri.fillColor;
		out_tri2.lineColor = in_tri.lineColor;

		out_tri2.points[0] = *inside_points[0];
		out_tri2.points[1] = out_tri1.points[2];
		out_tri2.points[2] = VectorIntersectPlane(P_point, P_normal, *inside_points[1], *outside_points[0]);

		return 2;
	}

}