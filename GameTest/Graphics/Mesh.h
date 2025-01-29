#pragma once

#include <vector>
#include <fstream>
#include <sstream>

#include "Triangle.h"
#include "App/main.h"

class Mesh
{
	bool normalExist = false;

public:	
	Mesh();
	Mesh(std::string filename);
	std::vector<Triangle> triangles;

	void LoadFromObj(std::string filename);
	//int ParseObj(std::string filename, std::string filename2);

	Vec3 pos = { 0.0f, 0.0f, 0.0f };
	Vec3 scale = { 1.0f, 1.0f, 1.0f };
	Vec3 color = { 1.0f, 1.0f, 1.0f };

	float fYaw = 0.0f, fPitch = 0.0f, fRoll = 0.0f;

	Vec3 min_corner = { INFINITY, INFINITY, INFINITY };
	Vec3 max_corner = { -INFINITY, -INFINITY, -INFINITY };

};