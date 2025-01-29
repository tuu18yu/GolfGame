#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
{
	
	Type t = CUBE;
	Mesh cube;
	cube.LoadFromObj("./Data/cube.obj");
	meshes[t] = cube;

	t = SPHERE;
	Mesh sphere;
	sphere.LoadFromObj("./Data/sphere.obj");
	meshes[t] = sphere;

	t = CAPSULE;
	Mesh capsule;
	capsule.LoadFromObj("./Data/capsule.obj");
	meshes[t] = capsule;

	t = CYLINDER;
	Mesh cylinder;
	cylinder.LoadFromObj("./Data/cylinder.obj");
	meshes[t] = cylinder;

	t = PLANE;
	Mesh plane;
	plane.LoadFromObj("./Data/plane.obj");
	meshes[t] = plane;

	t = QUAD;
	Mesh quad;
	quad.LoadFromObj("./Data/quad.obj");
	meshes[t] = quad;

	t = TEAPOT;
	Mesh axis;
	axis.LoadFromObj("./Data/teapot.obj");
	meshes[t] = axis;

}
