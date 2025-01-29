#include "stdafx.h"
#include "Particle.h"

void ParticleSystem::resetParticles(size_t count, float size)
{
	m_particles = std::vector<Particle>(count);
	vertexArray = std::vector<Triangle>(count);
	m_size = size;
	//m_pos = pos;
	//m_velocity = velocity;

	for (size_t i = 0; i < count; i++)
	{
		resetParticle(i);
	}
}


void ParticleSystem::update(const float deltaTime)
{
	for (size_t i = 0; i < m_particles.size(); i++)
	{
		if (m_particles[i].lifeTime < 0.0f)
		{
			resetParticle(i);
		}

		vertexArray[i].points[0] += m_particles[i].velocity * deltaTime;
		vertexArray[i].points[1] += m_particles[i].velocity * deltaTime;
		vertexArray[i].points[2] += m_particles[i].velocity * deltaTime;

		m_particles[i].lifeTime -= deltaTime;
	}
}

void ExplosiveParticle::resetParticle(size_t i)
{
	// give the particle an initial position
	vertexArray[i].points[0] = {m_pos.x, m_pos.y + m_size, m_pos.z};
	vertexArray[i].points[1] = { m_pos.x + m_size, m_pos.y - m_size, m_pos.z };
	vertexArray[i].points[2] = { m_pos.x - m_size, m_pos.y - m_size, m_pos.z };

	// give the particle a color
	
	float r = ((float)rand() / RAND_MAX);
	float g = ((float)rand() / RAND_MAX);
	float b = ((float)rand() / RAND_MAX);
	
	Vec3 color = { r, g, b };
	//Vec3 color = { 0.529f, 0.357f, 0.71f };
	vertexArray[i].lineColor = color;
	vertexArray[i].fillColor = color;

	// give the particle a random inten
	float rx = ((float)rand() / RAND_MAX) * m_velocity.x - (m_velocity.x / 2.0f);
	float ry = ((float)rand() / RAND_MAX) * m_velocity.y - (m_velocity.y / 2.0f);
	float rz = ((float)rand() / RAND_MAX) * m_velocity.z - (m_velocity.z / 2.0f);
	m_particles[i].velocity = { rx, ry, rz };

	// give the particle a lifespan
	m_particles[i].lifeTime = static_cast<float>(rand() % m_life);
}

void DirectionalParticle::resetParticle(size_t i)
{
	// give the particle an initial position
	vertexArray[i].points[0] = { m_pos.x, m_pos.y + m_size, m_pos.z };
	vertexArray[i].points[1] = { m_pos.x + m_size, m_pos.y - m_size, m_pos.z };
	vertexArray[i].points[2] = { m_pos.x - m_size, m_pos.y - m_size, m_pos.z };

	// give particle shade of green
	float r = m_color.x * (((float)rand() / RAND_MAX) + 0.2f);
	float g = m_color.y * (((float)rand() / RAND_MAX) + 0.2f);
	float b = m_color.z * (((float)rand() / RAND_MAX) + 0.2f);
	Vec3 color = { r, g, b };
	//Vec3 color = { 0.529f, 0.357f, 0.71f };
	vertexArray[i].lineColor = color;
	vertexArray[i].fillColor = color;

	// give the particle a random inten
	if (axis == 0) // x
	{
		float rx = ((float)rand() / RAND_MAX * m_velocity.x) - (m_velocity.x / 2.0f);
		float ry = ((float)rand() / RAND_MAX * m_velocity.y) - (m_velocity.x / 3.0f);
		float rz = ((float)rand() / RAND_MAX * m_velocity.z - (m_velocity.x / 2.0f)) * 2.0f;
		m_particles[i].velocity = { rx, ry, rz };
	}
	else if (axis == 2) // z
	{
		float rx = ((float)rand() / RAND_MAX) * m_velocity.x - (m_velocity.x / 2.0f) * 2.0f;
		float ry = ((float)rand() / RAND_MAX * m_velocity.y) - (m_velocity.x / 3.0f);
		float rz = ((float)rand() / RAND_MAX * m_velocity.z) - (m_velocity.x / 2.0f);
		m_particles[i].velocity = { rx, ry, rz };
	}

	// give the particle a lifespan
	m_particles[i].lifeTime = static_cast<float>(rand() % m_life);
}

void DirectionalParticle::update(const float deltaTime)
{
	for (size_t i = 0; i < m_particles.size(); i++)
	{
		if (m_particles[i].lifeTime < 0.0f)
		{
			// set the triangle to same point do it doesn't get drawn
			vertexArray[i].points[0] = { m_pos.x, m_pos.y, m_pos.z };
			vertexArray[i].points[1] = { m_pos.x, m_pos.y, m_pos.z };
			vertexArray[i].points[2] = { m_pos.x, m_pos.y, m_pos.z };
		}

		vertexArray[i].points[0] += m_particles[i].velocity * deltaTime;
		vertexArray[i].points[1] += m_particles[i].velocity * deltaTime;
		vertexArray[i].points[2] += m_particles[i].velocity * deltaTime;

		m_particles[i].lifeTime -= deltaTime;
	}
}
