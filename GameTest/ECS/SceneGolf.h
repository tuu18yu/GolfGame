#pragma once

#include "Scene.h"
#include "Graphics/Particle.h"
#include "App/app.h"
#include "Math/Vec2.h"

class SceneGolf: public Scene
{
	ExplosiveParticle m_particleGoal;
	DirectionalParticle m_particleHit;

	Cooldown m_redPortalCoolDown;
	Cooldown m_bluePortalCoolDown;

	const float velocityThreshold = 0.05f;
	const float heightThreshold = -5.0f;
	const float maxDeltaTime = 0.016f; // approx 60 FPS

	float redPortalTime = 8.0f; // single when playing multiplayer do 20 and 25
	float bluePortalTime = 12.0f;

	const Vec3 m_gravity = { 0.0f, -9.81f, 0.0f }; // acceleration
	const float m_drag = 0.82f; // Netwton
	const float m_lift = 1.97f; // Netwton

	const Vec3 m_ballColor = { 0.98f, 0.988f, 0.886f };
	const Vec3 m_ball2Color = { 0.631f, 0.439f, 0.729f };

	const Vec3 m_collideColor = { 0.941f, 0.204f, 0.388f };
	const Vec3 m_grassColor = { 0.071f, 0.459f, 0.224f };
	const Vec3 m_sandColor = { 0.831f, 0.725f, 0.341f };
	const Vec3 m_goalColor = { 0.38f, 0.0f, 0.0f };
	const Vec3 m_redColor = { 1.0f, 0.592f, 0.592f };
	const Vec3 m_blueColor = { 0.18f, 0.208f, 1.0f };
	const Vec3 m_movingColor = { 0.78f, 1.0f, 0.69f };

	const int m_total_lives = 3;

	int m_toal_moves = 0;
	//bool m_turnEnded = false;

	size_t m_pID = -1;
	size_t m_pID2 = -1;
	size_t m_currentPlayer = -1;

	bool m_isPutterPressed = false;
	bool m_isPutter = false;

	bool m_isShotPressed = false;
	bool m_isShotReleased = false;

	bool m_isMultiplayer = false;
	bool m_isOnePlayerScene = false;
	bool m_isGamePlaying = false;
	bool m_menu = true;
	bool m_didWin = false;

	float m_intensity = 0.0f;
	float m_intensityIncrease = 0.025f;

	Vec3 m_startPostion = { 0.0f, 0.0f, 0.0f };

	// SceneGolfEntity.cpp
	void createMap(std::string filename);
	size_t createBall(Vec3 pos, Vec3 scale, const Vec3 color);
	void createFlag(Vec3 pos, Vec3 scale);
	void createFloor(Vec3 pos, Vec3 scale, const std::string& type, Vec3 endPos = {0.0f, 0.0f, 0.0f});
	void respawn(const size_t playerID);
	void spawnPortal(const size_t playerID, const Vec3 pos);
	void resetCamera(const size_t playerID);

	// SceneGolfPhysics.cpp
	void sApplyForce(const float deltaTime, const size_t playerID);
	void sUpdatePosition(const float deltaTime, const size_t playerID);
	void sMoveBlocks(const float deltaTime, const size_t blockID);

	// SceneGolfCollision.cpp
	void sBallCollision(const size_t playerID);
	void levelCollision(size_t ID1, size_t ID2);
	void ballCollision(size_t ID1, size_t ID2);

	// SceneGolfHandleInput.cpp
	void sGetInput(const size_t playerID);
	void moveCamera(CTransform& trans, CInput& input);
	void getShotIntensity(CInput& input);
	void getMenuInput();

	// SceneGolfUI.cpp
	void displayIntensity();
	void displayDriverType() const;
	void displayPlayerStatus(const Vec2& pos, const size_t ID) const;
	void displayMenu();
	void displayResult();

	// SceneGolfRender.cpp
	void drawEntities() const;
	void drawBalls() const;
	void drawBall(const size_t ID) const;

public:
	SceneGolf() {}
	void Init() override;
	void Render() override;
	void Update(const float deltaTime) override;
	void Shutdown() override;
};