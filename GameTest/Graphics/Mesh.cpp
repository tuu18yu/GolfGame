#include "stdafx.h"

#include "Mesh.h"
#include <algorithm>
#include "../App/app.h"

Mesh::Mesh()
{
}

Mesh::Mesh(std::string filename)
{
	LoadFromObj(filename);
}

void Mesh::LoadFromObj(std::string filename)
{
	std::ifstream file(filename);

	// Local cache of vertices, normals, and triangles
	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;

	while (!file.eof())
	{
		char line[128];
		file.getline(line, 128);

		std::stringstream string;
		string << line;

		char first_letter;
		if (line[0] == 'v' && line[1] == ' ')
		{
			Vec3 vertex;
			string >> first_letter >> vertex.x >> vertex.y >> vertex.z;
			vertices.push_back(vertex);

			// Get the maximum and minimum x, y, z values
			if (vertex.x > max_corner.x) { max_corner.x = vertex.x; }
			if (vertex.y > max_corner.y) { max_corner.y = vertex.y; }
			if (vertex.z > max_corner.z) { max_corner.z = vertex.z; }

			if (vertex.x < min_corner.x) { min_corner.x = vertex.x; }
			if (vertex.y < min_corner.y) { min_corner.y = vertex.y; }
			if (vertex.z < min_corner.z) { min_corner.z = vertex.z; }
		}
		else if (line[0] == 'v' && line[1] == 'n')
		{
			normalExist = true;
			Vec3 normal;
			string >> first_letter >> first_letter >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (line[0] == 'f')
		{
			int indices[3] = { 0 };
			int normalIndices[3] = { 0 };
			int texCoordIndices[3] = { 0 }; // Optional texture coordinates
			char delimiter;

			// Parse face data
			string >> first_letter;
			for (int i = 0; i < 3; ++i)
			{
				// Parse vertex index
				string >> indices[i];

				// Check for '/' indicating additional data
				if (string.peek() == '/')
				{
					string >> delimiter; // Consume '/'

					// Check for texture coordinate index
					if (string.peek() != '/')
					{
						string >> texCoordIndices[i];
					}

					// Check for normal index
					if (string.peek() == '/')
					{
						string >> delimiter; // Consume '/'
						string >> normalIndices[i];
					}
				}
			}

			Triangle face(vertices[indices[0] - 1], vertices[indices[1] - 1], vertices[indices[2] - 1]);
			
			// Add normals if present
			if (normalIndices[0] && normalIndices[1] && normalIndices[2])
			{
				face.normals[0] = normals[normalIndices[0] - 1];
				face.normals[1] = normals[normalIndices[1] - 1];
				face.normals[2] = normals[normalIndices[2] - 1];

				if (face.normals[0].normalize() != face.GetNormal())
				{
					face.isCW = false;
				}
			}

			triangles.push_back(face);
		}
	}
}


//int Mesh::ParseObj(std::string filename, std::string filename2)
//{
//	std::ifstream file(filename);
//	std::ofstream output(filename2);
//
//	if (!file.is_open()) { return -1; }
//
//	while (!file.eof())
//	{
//		char line[128];
//		file.getline(line, 128);
//
//		std::stringstream string;
//		string << line;
//
//		char first_letter;
//		if (line[0] == 'v')
//		{
//			output << line << "\n";
//		}
//
//		else if (line[0] == 'f')
//		{
//			std::string newLine;
//			std::string indicies[3];
//			string >> first_letter >> indicies[0] >> indicies[1] >> indicies[2];
//
//			newLine += first_letter;
//
//			for (std::string& ind : indicies)
//			{
//				std::vector<std::string> tokens;
//				std::string token;
//				std::stringstream ss(ind);
//
//				while (std::getline(ss, token, '/')) {
//					tokens.push_back(token);
//				}
//
//				newLine += " " + tokens[0];
//			}
//
//			output << newLine << "\n";
//
//		}
//	}
//	output.close();
//}