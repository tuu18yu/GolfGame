#pragma once

#include "EntityManager.h"
#include "../Graphics/Renderer.h"

#include <array>


class Scene
{
protected:

	Camera camera;
	Renderer renderer;

	EntityManager	m_entityManager;
	std::array<Mesh, 7> meshes; // I am using array as meshes is static
	enum Type {
		CUBE = 0,
		SPHERE = 1,
		CAPSULE = 2,
		CYLINDER = 3,
		PLANE = 4,
		QUAD = 5,
		TEAPOT = 6
	};

public:
	Scene();

	virtual void Update(const float deltaTime) = 0;
	virtual void Init() = 0;
	virtual void Render() = 0;
	virtual void Shutdown() = 0;
};