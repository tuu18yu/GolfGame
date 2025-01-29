#include "stdafx.h"

#include "ECS/SceneThirdPerson.h"
#include "App/app.h"

#include <algorithm>

SceneThirdPerson::SceneThirdPerson()
{
}

void SceneThirdPerson::createMap(std::string filename)
{
	Vec3 color = { 0.467f, 0.78f, 0.761f };

	//Vec3 pos = { 0.0f, 0.0f, -13.0f };
	//Vec3 scale = { 7.0f, 0.1f, 7.0f };
	//// create floor
	//size_t ID = m_entityManager.addEntity("floor");
	//CTransform& trans = m_entityManager.getComponentVector<CTransform>()[ID];
	//trans.has = true;
	//trans.pos = pos;
	//trans.scale = scale;
	////trans.fPitch = -90.f;
	////trans.fRoll = 90.0f;

	//CMesh& mesh = m_entityManager.getComponentVector<CMesh>()[ID];
	//mesh.has = true;
	//mesh.type = CUBE;
	//mesh.color = color;

	//CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[ID];
	//box.has = true;
	//box.pos = pos;
	//box.scale = scale;
	//box.setMaxCorner(meshes[CUBE].max_corner);
	//box.setMinCorner(meshes[CUBE].min_corner);

	//m_entityManager.addEntityToBVH(ID);

	std::ifstream file(filename);

	
	int row = 0, col = 0;
	int currCol = 0;

	while (!file.eof())
	{
		char line[128] = "";
		file.getline(line, 128);

		std::stringstream string;
		string << line;
		char first_letter;
		if (line[0] == 'r')
		{
			string >> first_letter >> row;
		}
		else if (line[0] == 'c')
		{
			string >> first_letter >> col;
		}

		else
		{
			for (int i = 0; i < row; i++)
			{
				if (line[i] == '*')
				{
					if (i < row / 2)
					{
						createWall({ 0.0f - (row / 2 - i), 0.0f, -10.0f - (col - currCol)}, {1.0f, 1.0f, 1.0f}, color);
					}

					else
					{
						createWall({ 0.0f + (i - row / 2), 0.0f, -10.0f - (col - currCol) }, { 1.0f, 1.0f, 1.0f }, color);
					}
				}

				// create player
				else if (line[i] == 'p')
				{
					m_pID = m_entityManager.addEntity("player");
					Vec3 playerPos;
					//playerPos = {0.0f, 0.0f, -9.0f};
					if (i < row / 2)
					{
						playerPos = Vec3{ 0.0f - (row / 2 - i), 0.0f, -10.0f - (col - currCol) };
					}
					else
					{
						playerPos = Vec3{ 0.0f + (i - row / 2), 0.0f, -10.0f - (col - currCol) };
					}

					CTransform& trans = m_entityManager.getComponentVector<CTransform>()[m_pID];
					trans.has = true;
					trans.pos = playerPos;
					//trans.fYaw = 10.0f;
					trans.scale = { 0.45f, 0.45f, 0.45f };

					CMesh& mesh = m_entityManager.getComponentVector<CMesh>()[m_pID];
					mesh.has = true;
					mesh.type = CAPSULE;
					mesh.color = { 0.467f, 0.68f, 0.261f };

					CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[m_pID];
					box.has = true;
					box.pos = playerPos;
					box.scale = { 0.45f, 0.45f, 0.45f };
					box.setMaxCorner(meshes[CAPSULE].max_corner);
					box.setMinCorner(meshes[CAPSULE].min_corner);

					m_entityManager.addEntityToBVH(m_pID);
				}

			}

			currCol++;
		}
	}
}

void SceneThirdPerson::createWall(Vec3 pos, Vec3 scale, Vec3 color)
{
	size_t ID = m_entityManager.addEntity("wall");
	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[ID];
	trans.has = true;
	trans.pos = pos;
	trans.scale = scale;
	trans.fPitch = 90.f;

	CMesh& mesh = m_entityManager.getComponentVector<CMesh>()[ID];
	mesh.has = true;
	mesh.type = CUBE;
	mesh.color = color;

	CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[ID];
	box.has = true;
	box.pos = pos;
	box.scale = scale;
	box.setMaxCorner(meshes[CUBE].max_corner);
	box.setMinCorner(meshes[CUBE].min_corner);

	m_entityManager.addEntityToBVH(ID);

}


void SceneThirdPerson::Init()
{
	createMap("Data/maze1.txt");
}

void SceneThirdPerson::sInput(const float deltaTime)
{

	Vec3 pos = { 0.0f, 0.0f, 0.0f };
	//float yaw = 0.0f, pitch = 0.0f;
	float speed = 0.003f;

	// Keyboard Contol
	if (App::IsKeyPressed('W')) // up
	{
		//pos += camFront * (speed * deltaTime);
		pos.z -= (speed * deltaTime);
	}

	if (App::IsKeyPressed('S')) // down
	{
		//pos -= camFront * (speed * deltaTime);
		pos.z += (speed * deltaTime);
	}

	if (App::IsKeyPressed('D')) // left
	{
		//pos += (camFront.cross_product(camUp)).normalize() * (speed * deltaTime);
		pos.x += (speed * deltaTime);
	}
	if (App::IsKeyPressed('A')) // right
	{
		//pos -= (camFront.cross_product(camUp)).normalize() * (speed * deltaTime);
		pos.x -= (speed * deltaTime);
	}

	if (App::IsKeyPressed(VK_SPACE)) // right
	{
		//pos -= (camFront.cross_product(camUp)).normalize() * (speed * deltaTime);
		pos.y += (speed * deltaTime);
	}

	if (App::IsKeyPressed('B')) // right
	{
		//pos -= (camFront.cross_product(camUp)).normalize() * (speed * deltaTime);
		pos.y -= (speed * deltaTime);
	}

	//if (App::IsKeyPressed(VK_UP)) // up
	//{
	//	//pos += camFront * (speed * deltaTime);
	//	camera.pos.z -= (speed * deltaTime);
	//}

	//if (App::IsKeyPressed(VK_DOWN)) // down
	//{
	//	//pos -= camFront * (speed * deltaTime);
	//	camera.pos.z += (speed * deltaTime);
	//}


	//// Mouse Control
	//float mouseX, mouseY;
	//App::GetMousePos(mouseX, mouseY);
	//float space = 100.f;
	//float halfWidth = WINDOW_WIDTH / 2.0f;
	//float halfHeight = WINDOW_HEIGHT / 2.0f;

	//float sensitivity = 0.0001f;

	//// Check if mouse is within the game screen
	//if ((mouseX >= 0.0f) && (mouseX <= WINDOW_WIDTH) && (mouseY >= 0.0f) && (mouseY <= WINDOW_HEIGHT))
	//{
	//	if ((mouseY > halfHeight + space) && (halfWidth - space < mouseX < halfWidth + space)) // up
	//	{
	//		//pos.z +=  (speed * deltaTime);
	//		pitch += sensitivity * deltaTime;
	//	}
	//	else if ((mouseY < halfHeight - space) && (halfWidth - space < mouseX < halfWidth + space)) // down
	//	{
	//		//pos.z -= (speed * deltaTime);
	//		pitch -= sensitivity * deltaTime;
	//	}

	//	if ((mouseX > halfWidth + space) && (halfHeight - space < mouseY < halfHeight + space)) // left
	//	{
	//		yaw += sensitivity * deltaTime;
	//	}
	//	else if ((mouseX < halfWidth - space) && (halfHeight - space < mouseY < halfHeight + space)) // right
	//	{
	//		yaw -= sensitivity * deltaTime;
	//	}
	//}


	/*if (-90.f < (camera.fYaw + yaw) < 90.0f) { camera.fYaw += yaw; }
	if (-90.f < (camera.fPitch + pitch) < 90.0f) { camera.fPitch += pitch; }*/
	
	CTransform &cTrans = m_entityManager.getComponentVector<CTransform>()[m_pID];
	cTrans.pos += pos;



	//camera.forward = cTrans.pos;

	if (App::IsKeyPressed(VK_LEFT)) // left
	{
		//pos += (camFront.cross_product(camUp)).normalize() * (speed * deltaTime);

		Mat4 rotateY;
		rotateY.YRotate(-90.0f);
		camera.forward = rotateY * camera.forward;

		camera.pos.x = cTrans.pos.x - 25.0f;
		camera.pos.y = cTrans.pos.y + 15.0f;
	}
	else if (App::IsKeyPressed(VK_RIGHT)) // right
	{
		//pos -= (camFront.cross_product(camUp)).normalize() * (speed * deltaTime);
		Mat4 rotateY;
		rotateY.YRotate(90.0f);
		camera.forward = rotateY * camera.forward;

		camera.pos.x = cTrans.pos.x + 25.0f;
		camera.pos.y = cTrans.pos.y + 15.0f;
	}
	else
	{
		camera.forward = cTrans.pos;

		camera.pos.z = cTrans.pos.z + 25.0f;
		camera.pos.y = cTrans.pos.y + 15.0f;

	}


	CBoundingBox& cBox = m_entityManager.getComponentVector<CBoundingBox>()[m_pID];
	cBox.pos = cTrans.pos;
	m_entityManager.getBVH().UpdateEntityPosition(m_pID, m_entityManager);
	// camera.pos

	/*size_t ID = -1;
	float t = 0.0f;
	Ray ray(cTrans.pos, { 0.0f, 0.0f, -1.0f });
	if (m_entityManager.getBVH().RayCast(ray, ID, t, m_pID))
	{
		CMesh cmesh = m_entityManager.getComponentVector<CMesh>()[ID];
		cmesh.color = { 0.5f, 0.2f, 0.7f };
	}*/
}

void SceneThirdPerson::Update(const float deltaTime)
{
	/*CTransform& playerTrans = m_entityManager.getComponentVector<CTransform>()[m_pID];
	playerTrans.fYaw += 0.4f;
	playerTrans.fRoll -= 0.2f;*/

	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[m_pID];
	CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[m_pID];
	sInput(deltaTime);
	sPlayerCollision();
	sRayCast(box);
}

void SceneThirdPerson::Render()
{
	std::vector<int> indicies;
	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		CMesh cmesh = m_entityManager.getComponentVector<CMesh>()[i];
		CTransform cTrans = m_entityManager.getComponentVector<CTransform>()[i];
		if (cmesh.has && cTrans.has)
		{
			indicies.push_back(i);
		}
	}

	// Sort triangle in order of z, z is ordered based on average z value
	std::sort(indicies.begin(), indicies.end(), [&](int i1, int i2)
		{
			CTransform cTrans1 = m_entityManager.getComponentVector<CTransform>()[i1];
			CTransform cTrans2 = m_entityManager.getComponentVector<CTransform>()[i2];
			return  cTrans1.pos.z < cTrans2.pos.z;
		});

	//camera.SetCamera();

	for (int i : indicies)
	{
		CMesh cmesh = m_entityManager.getComponentVector<CMesh>()[i];
		CTransform cTrans = m_entityManager.getComponentVector<CTransform>()[i];

		Mesh mesh = meshes[cmesh.type];
		mesh.scale = cTrans.scale;
		mesh.pos = cTrans.pos;
		mesh.color = cmesh.color;
		mesh.fYaw = cTrans.fYaw;
		mesh.fRoll = cTrans.fRoll;
		mesh.fPitch = cTrans.fPitch;

		renderer.Draw(camera, mesh, false);

	}

	// draw aabb tree
	/*for (auto& node : m_entityManager.getBVH().nodes)
	{
		renderer.Draw(camera, node.aabb);
	}*/

}

void SceneThirdPerson::sRayCast(CBoundingBox& box)
{
	size_t ID = -1;
	float t = 0.0f;
	Vec3 rayPos = box.min_corner() + ((box.max_corner() - box.min_corner()) / 2.0f);
	Ray ray(rayPos, {0.0f, 0.0f, -1.0f});

	if (m_entityManager.getBVH().RayCast(ray, ID, t, m_pID))
	{
		CMesh& cmesh = m_entityManager.getComponentVector<CMesh>()[ID];
		cmesh.color = m_rayColor;
	}
}


void SceneThirdPerson::sPlayerCollision()
{
	for (size_t i = 0; i < MAX_ENTITIES; i++)
	{
		if ("wall" == m_entityManager.getTag(i))
		{
			CMesh& cmesh = m_entityManager.getComponentVector<CMesh>()[i];
			cmesh.color = m_blockColor;
		}
	}

	std::vector<size_t> indicies;
	// get index of all the objects that collide with player
	CBoundingBox& cBox = m_entityManager.getComponentVector<CBoundingBox>()[m_pID];
	CState& state = m_entityManager.getComponentVector<CState>()[m_pID];

	m_entityManager.getBVH().Query(cBox.nodeID, indicies);

	// if no block is being touched not on the ground
	if (indicies.size() == 0) { state.isGrounded = false; }

	for (auto& i : indicies)
	{
		CMesh& cmesh = m_entityManager.getComponentVector<CMesh>()[i];
		cmesh.color = m_collideColor;

		{
			// Collision resolution
			sResolveCollision(m_pID, i);
		}
	}

}


void SceneThirdPerson::sResolveCollision(size_t ID1, size_t ID2)
{
	CBoundingBox& box1 = m_entityManager.getComponentVector<CBoundingBox>()[ID1];
	CBoundingBox& box2 = m_entityManager.getComponentVector<CBoundingBox>()[ID2];

	CTransform& trans1 = m_entityManager.getComponentVector<CTransform>()[ID1];
	CTransform& trans2 = m_entityManager.getComponentVector<CTransform>()[ID2];

	CRigidBody& rb = m_entityManager.getComponentVector<CRigidBody>()[ID1];
	CState& state = m_entityManager.getComponentVector<CState>()[ID1];

	// Issue: when block is being collided while in air, and overlap in Y axis it pushes the object up

	// Calculate overlap along each axis
	float overlapX = min(box1.max_corner().x, box2.max_corner().x) - max(box1.min_corner().x, box2.min_corner().x);
	float overlapZ = min(box1.max_corner().z, box2.max_corner().z) - max(box1.min_corner().z, box2.min_corner().z);


	// Resolve overlap based on the smallest penetration axis
	// Note that we only need to adjust on one side, pushing the player
	if (overlapX < overlapZ)
	{
		// Horizontal collision

		// left of the box
		if (box1.min_corner().x < box2.min_corner().x)
		{
			trans1.pos.x -= overlapX;
			//rb.velocity.x = 0.0f;
			//state.isDashing = false;
		}
		// right of the box
		else
		{
			trans1.pos.x += overlapX;
			//rb.velocity.x = 0.0f;
			//state.isDashing = false;
		}

		// Reverse velocity on collision axis
		//RB1.velocity.x = -RB1.velocity.x * 0.5f; // Dampen velocity slightly
	}
	else
	{
		// Vertical collision

		// below the box collided
		if (box1.min_corner().z < box2.min_corner().z)
		{
			trans1.pos.z -= overlapZ;
			// stop moving up velocity when box is blocking the jump
			//rb.velocity.y = 0.0f;
		}

		// above the box collided
		else
		{
			trans1.pos.z += overlapZ;
			//state.isGrounded = true;
		}

		// Reverse velocity on collision axis
		//RB1.velocity.y = -RB1.velocity.y * 0.5f; // Dampen velocity slightly
	}

	// Update bounding box positions
	box1.pos = trans1.pos;
	box2.pos = trans2.pos;

	m_entityManager.getBVH().UpdateEntityPosition(ID1, m_entityManager);
	m_entityManager.getBVH().UpdateEntityPosition(ID2, m_entityManager);
}

void SceneThirdPerson::Shutdown()
{
}