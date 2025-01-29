#pragma once

#include "Scene.h"

#include <vector>

class SceneSideScroll : public Scene
{
private:
	size_t m_root = 0;
	size_t m_pID = 0;
	Vec3 m_blockColor = { 0.467f, 0.78f, 0.761f };
	Vec3 m_collideColor = { 0.941f, 0.204f, 0.388f };
	Vec3 m_obstacleColor = { 0.678f, 0.161f, 0.161f };
	Vec3 m_teapotColor = { 0.71f, 0.702f, 0.165f };
	
	const int TOTAL_TEAPOTS = 3;

	const float DASH = 20.0f;
	const float DRAG = -15.0f;
	const float GRAVITY = -30.0f;         
	const float JUMP_FORCE = 10.0f;    
	const float MOVE_SPEED = 8.0f;
	const float DASH_COOLDOWN = 0.25f;
	const float RESPAWN_COOLDOWN = 0.15f;

	void createMap(std::string filename);
	void createPlayer(Vec3 pos, Vec3 scale, float mass);
	void createBlock(Vec3 pos, Vec3 scale, Vec3 color, bool isObstacle = false);
	void createTeapot(Vec3 pos, Vec3 scale, Vec3 color);

	void sPlayerCollision();
	void sRevive();

	void sInput(CInput &input, CState& state);
	void sUpdateVelocity(CRigidBody &rb, CInput& input, CTransform& trans, CState& state, const float deltaTime);
	void sUpdatePosition(CRigidBody& rb, CTransform& trans, CState& state, const float deltaTime);
	void sResolveCollision(size_t ID1, size_t ID2);
	void sRayCast(CTransform& trans);

public:
	SceneSideScroll();
	void Init() override;
	void Render() override;
	void Update(const float deltaTime) override;
	void Shutdown() override;

};
