#pragma once
#include "Math/Vec3.h"
#include "Graphics/Triangle.h"
#include "Physics/AABBTree.h"

struct Cooldown
{
	float timer = 0.0f;

	void update(const float deltaTime)
	{
		if (timer > 0.0f) { timer -= deltaTime; }
	}

	bool isReady() const
	{
		return timer <= 0.0f;
	}

	void reset(const float duration)
	{
		timer = duration;
	}

	Cooldown() {}
};
// Return the point where line intersects with plane
// point on a plane, normal of a plane, point where line starts, point where line ends 
Vec3 VectorIntersectPlane(Vec3 const& P_point, Vec3 const& P_normal, Vec3 const& lineStart, Vec3 const& lineEnd);
// Return number of triangles that has been formed
int TriangleClipAgainstPlane(Vec3 const& P_point, Vec3 const& P_normal, Triangle& in_tri, Triangle& out_tri1, Triangle& out_tri2);