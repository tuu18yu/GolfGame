#pragma once
#include "../Math/Vec3.h"
#include "Triangle.h"
#include <vector>

class ParticleSystem
{
protected:
	struct Particle
	{
		Vec3 velocity;
		float lifeTime = 0.0f;
	};

	std::vector<Particle> m_particles;

	float m_size = 0.001f;
	Vec3 m_pos = { 0.0f, 0.0f, 0.0f };
	Vec3 m_color = { 0.071f, 0.459f, 0.224f };
	Vec3 m_velocity = { 10.0f, 10.0f, 10.0f };
	int m_life = 20;

	//void resetParticles(size_t count = 100, float size = 0.01f);
	virtual void resetParticle(size_t i) = 0;
	//void update();

public:
	std::vector<Triangle> vertexArray;

	ParticleSystem() {}
	void update(const float deltaTime);
	void resetParticles(size_t count = 1500, float size = 0.01f);
	void setPosition(const Vec3 pos) { m_pos = pos; }
	void setColor(const Vec3 color) { m_pos = color; }
	void setDuration(const int length) { m_life = length; }
	void setVelocity(const Vec3 vel) { m_velocity = vel; }
};

class ExplosiveParticle : public ParticleSystem
{
protected:
		void resetParticle(size_t i) override;
};

class DirectionalParticle: public ParticleSystem
{
protected:
	void resetParticle(size_t i) override;
public:
	int axis = -1;
	void update(const float deltaTime);
};