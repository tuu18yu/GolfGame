#pragma once

#include "../Utils/Utils.h"
#include "./Math/Vec3.h"
#include <string>

// All component class "has" variable that determines whether entity has or not the corresponding component
class Component
{
public:
	bool has = false;
};

class CRigidBody : public Component
{
public:
	float mass = 1.0f;

	Vec3 velocity = { 0.0f, 0.0f, 0.0f };
	Vec3 force = { 0.0f, 0.0f, 0.0f };
	
	// coefficient of friction
	float friction = 0.0f;
	CRigidBody() {}
};

class CTransform : public Component
{
public:
	Vec3 forward = { 0.0f, 0.0f, 0.0f };
	Vec3 pos = { 0.0f, 0.0f, 0.0f };
	Vec3 prevPos = { 0.0f, 0.0f, 0.0f };
	Vec3 scale = { 1.0f, 1.0f, 1.0f };

	float fYaw = 0.0f, fPitch = 0.0f, fRoll = 0.0f;

	CTransform() {}
	CTransform(const Vec3& p)
		: pos(p) {
	}
	CTransform(const Vec3& p, const Vec3& sc, float a, float yaw, float pitch, float roll)
		: pos(p), prevPos(p), scale(sc), fYaw(yaw), fPitch(pitch), fRoll(roll) {
	}
};

class CLifeSpan : public Component
{
public:
	int lifespan = 0;
	int frameCreated = 0;
	CLifeSpan() {}
	CLifeSpan(int duration, int frame)
		: lifespan(duration), frameCreated(frame) {
	}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;

	CInput() {}
};



class CBoundingBox : public Component
{
private:
	// intial max and min corner are corners when mesh is postioned at (0, 0, 0)
	// and scaled by 1
	// they are used to compute the actual bounding box

	Vec3 m_min_corner = { 0.0f, 0.0f, 0.0f }; // bottom, left, far
	Vec3 m_max_corner = { 0.0f, 0.0f, 0.0f }; // top, right, near
public:
	// one of the issue is updating the AABB position of corresponding node in the BVH Tree
	// solve be referencing the index of the node in BVH.nodes
	// faster than traversing through entire tree
	size_t nodeID = -1;
	Vec3 pos = { 0.0f, 0.0f, 0.0f };
	Vec3 scale = { 1.0f, 1.0f, 1.0f };

	inline void setMaxCorner(Vec3 max) { m_max_corner = max; }
	inline void setMinCorner(Vec3 min) { m_min_corner = min; }

	inline Vec3 max_corner() const { return m_max_corner * scale + pos; }
	inline Vec3 min_corner() const { return m_min_corner * scale + pos; }

	CBoundingBox() {}
	CBoundingBox(const Vec3& min_corner, const Vec3& max_corner) 
		: m_min_corner(min_corner), m_max_corner(max_corner) {}
};

class CState : public Component
{
public:
	bool isAlive = true;
	bool isGrounded = false;
	bool isShot = false;
	bool isCollided = false;
	bool turnEnded = false;
	size_t isOnMovingBlock = -1;

	int totalLives = 0;
	int totalMoves = 0;

	Cooldown spawnCoolDown;
	Vec3 spawnPos = { 0.0f, 0.0f, 0.0f };
	Vec3 startPos = { 0.0f, 0.0f, 0.0f };

	CState() {}
};

class CMesh : public Component
{
public:
	int type = -1;
	Vec3 color = {0.0f, 0.0f, 0.0f };
	CMesh() {}
	CMesh(int meshID)
		: type(meshID)
	{
	}
};

class CDamage : public Component
{
public:
	int amount = 1;
	CDamage() {}
};