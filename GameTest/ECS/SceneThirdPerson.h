#pragma once

#include "Scene.h"

#include <vector>

class SceneThirdPerson : public Scene
{
protected:
	size_t m_pID = 0;
	float radius = 10.0f;

	Vec3 m_blockColor = { 0.467f, 0.78f, 0.761f };
	Vec3 m_collideColor = { 0.941f, 0.204f, 0.388f };
	Vec3 m_rayColor = { 1.0f, 0.984f, 0.0f };

	void sInput(const float deltaTime);
	void createMap(std::string filename);
	void createWall(Vec3 pos, Vec3 scale, Vec3 color);
	void sPlayerCollision();
	void sResolveCollision(size_t ID1, size_t ID2);
	void sRayCast(CBoundingBox& box);
public:
	SceneThirdPerson();
	void Init() override;
	void Render() override;
	void Update(const float deltaTime) override;
	void Shutdown() override;
};