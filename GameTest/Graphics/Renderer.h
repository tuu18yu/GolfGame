#pragma once

#include "../Math/Vec3.h"
#include "../ECS/Component.h"
#include "../Physics/AABBTree.h"

#include "Mesh.h"
#include "Square.h"
#include "Particle.h"

class Camera
{
public:
	// Camera position
	Vec3 pos = { 0.0f, 0.0f, 0.0f };

	Vec3 forward = { 0.0f, 0.0f, -1.0f };
	Vec3 up = { 0.0f, 1.0f, 0.0f };
	Vec3 right = { 1.0f, 0.0f, 0.0f };

	float fYaw = 0.0f;
	float fPitch = 0.0f;

	Vec3 deltaMove = { 0.0f, 0.0f, 0.0f };

	Camera() {};
};

class Renderer
{
private:
	float m_fovAngle = 90.0f;
	float m_near = 0.1f;
	float m_far = 100.0f;

public:
	Renderer() {}
	void Draw(Camera camera, const Mesh &mesh, bool wireFrameOn = false) const;
	void Draw(Camera camera, const CBoundingBox &box) const;
	void Draw(Camera camera, const AABB& box) const;
	void Draw(Camera camera, const ParticleSystem& particleSystem) const;
	void Draw(Camera camera, const Ray& ray, float t) const;

	void Fill(const Triangle& triangle) const;
	void Fill(const Square& square) const;

	void ExtractFrustum(Frustum& frustum, const Camera camera) const;
};