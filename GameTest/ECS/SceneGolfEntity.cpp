#include "stdafx.h"
#include "SceneGolf.h"

void SceneGolf::createMap(std::string filename)
{
	std::ifstream file(filename);
	Vec3 scale(1.0f, 1.0f, 1.0f);

	while (!file.eof())
	{
		char line[128];
		file.getline(line, 128);

		std::stringstream string;
		string << line;

		char first_letter;
		if (line[0] == 'i')
		{
			Vec3 pos;
			string >> first_letter >> pos.x >> pos.y >> pos.z;
			m_startPostion = pos;
		}
		else if (line[0] == 'g')
		{
			Vec3 pos;
			string >> first_letter >> pos.x >> pos.y >> pos.z;
			createFloor(pos, scale, "grass");
		}
		else if (line[0] == 's')
		{
			Vec3 pos;
			string >> first_letter >> pos.x >> pos.y >> pos.z;
			createFloor(pos, scale, "sand");
		}
		else if (line[0] == 'f')
		{
			Vec3 pos;
			string >> first_letter >> pos.x >> pos.y >> pos.z;
			createFloor(pos, scale, "goal");
		}
		else if (line[0] == 'r')
		{
			Vec3 pos;
			Vec3 endPos;
			string >> first_letter >> pos.x >> pos.y >> pos.z >> endPos.x >> endPos.y >> endPos.z;
			endPos.y += 0.4f; // player should spawn above the block
			createFloor(pos, scale, "redPortal", endPos);
		}
		else if (line[0] == 'b')
		{
			Vec3 pos;
			Vec3 endPos;
			string >> first_letter >> pos.x >> pos.y >> pos.z >> endPos.x >> endPos.y >> endPos.z;
			endPos.y += 0.4f; // player should spawn above the block
			createFloor(pos, scale, "bluePortal", endPos);
		}
		else if (line[0] == 'm')
		{
			Vec3 pos;
			Vec3 endPos;
			string >> first_letter >> pos.x >> pos.y >> pos.z >> endPos.x >> endPos.y >> endPos.z;
			createFloor(pos, scale, "movingBlock", endPos);
		}
	}

	m_pID = createBall(m_startPostion, scale / 3.0f, m_ballColor);
}

size_t SceneGolf::createBall(Vec3 pos, Vec3 scale, const Vec3 color)
{
	size_t ID = m_entityManager.addEntity("ball");

	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[ID];
	trans.has = true;
	trans.pos = pos;
	trans.prevPos = { 0.0f, 0.0f, 0.0f };
	trans.scale = scale;
	trans.forward = { 0.0f, 0.0f, -1.0f };

	CMesh& mesh = m_entityManager.getComponentVector<CMesh>()[ID];
	mesh.has = true;
	mesh.type = SPHERE;
	mesh.color = color;

	CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[ID];
	box.has = true;
	box.pos = pos;
	box.scale = scale;
	box.setMaxCorner(meshes[SPHERE].max_corner);
	box.setMinCorner(meshes[SPHERE].min_corner);

	CInput& input = m_entityManager.getComponentVector<CInput>()[ID];
	input.has = true;

	CRigidBody& rb = m_entityManager.getComponentVector<CRigidBody>()[ID];
	rb.has = true;
	rb.mass = 0.45f;
	rb.velocity = { 0.0f, 0.0f, 0.0f };
	rb.force = { 0.0f, 0.0f, 0.0f };
	rb.friction = 0.0f;

	CState& state = m_entityManager.getComponentVector<CState>()[ID];

	state.isOnMovingBlock = -1;
	state.has = true;
	state.isGrounded = false;
	state.isAlive = true;
	state.isCollided = false;
	state.turnEnded = false;
	state.totalLives = m_total_lives;
	state.totalMoves = 0;
	state.spawnPos = pos; // set the initial spawn position to where it was first spawned when created

	m_entityManager.addEntityToBVH(ID);

	return ID;
}

void SceneGolf::createFlag(Vec3 pos, Vec3 scale)
{
	size_t ID = m_entityManager.addEntity("flag");

	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[ID];
	trans.has = true;
	trans.pos = pos;
	trans.scale = scale;

	CMesh& mesh = m_entityManager.getComponentVector<CMesh>()[ID];
	mesh.has = true;
	mesh.type = CYLINDER;
	mesh.color = m_goalColor;

	CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[ID];
	box.has = true;
	box.pos = pos;
	box.scale = scale;
	box.setMaxCorner(meshes[CYLINDER].max_corner);
	box.setMinCorner(meshes[CYLINDER].min_corner);


	m_entityManager.addEntityToBVH(ID);
}

void SceneGolf::createFloor(Vec3 pos, Vec3 scale, const std::string& type, Vec3 endPos)
{
	// create floor
	size_t ID = m_entityManager.addEntity(type);
	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[ID];
	trans.has = true;
	trans.pos = pos;
	trans.scale = scale;

	CMesh& mesh = m_entityManager.getComponentVector<CMesh>()[ID];
	mesh.has = true;
	mesh.type = CUBE;
	
	CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[ID];
	box.has = true;
	box.pos = pos;
	box.scale = scale;
	box.setMaxCorner(meshes[CUBE].max_corner);
	box.setMinCorner(meshes[CUBE].min_corner);

	CRigidBody& rb = m_entityManager.getComponentVector<CRigidBody>()[ID];
	rb.has = true;

	if (type == "grass")
	{
		mesh.color = m_grassColor;
		rb.friction = 0.5f;
	}

	else if (type == "sand")
	{
		mesh.color = m_sandColor;
		rb.friction = 2.0f;
	}
	else if (type == "goal")
	{
		mesh.color = m_goalColor;
		rb.friction = 1.0f;

		Vec3 pos2 = { pos.x, pos.y + 1.0f, pos.z };
		Vec3 scale2 = { 0.2f, 0.5f, 0.2f };
		createFlag(pos2, scale2);
	}
	else if (type == "redPortal")
	{
		mesh.color = m_redColor;
		rb.friction = 1.0f;

		CState& state = m_entityManager.getComponentVector<CState>()[ID];
		state.has = true;
		state.spawnPos = endPos;

	}
	else if (type == "bluePortal")
	{
		mesh.color = m_blueColor;
		rb.friction = 1.0f;

		CState& state = m_entityManager.getComponentVector<CState>()[ID];
		state.has = true;
		state.spawnPos = endPos;
	}
	else if (type == "movingBlock")
	{
		mesh.color = m_movingColor;
		rb.friction = 1.5f;

		CState& state = m_entityManager.getComponentVector<CState>()[ID];
		state.has = true;
		state.spawnPos = endPos;
		state.startPos = pos;
		
		trans.forward = (pos - endPos).normalize() * 0.75f;
		rb.velocity = trans.forward;
	}

	m_entityManager.addEntityToBVH(ID);
}

void SceneGolf::respawn(const size_t playerID)
{
	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[playerID];
	CRigidBody& rb = m_entityManager.getComponentVector<CRigidBody>()[playerID];
	CState& state = m_entityManager.getComponentVector<CState>()[playerID];

	// set state and rigidBody to default
	state.isGrounded = false;
	state.isCollided = false;
	state.isShot = false;

	rb.velocity = { 0.0f, 0.0f, 0.0f };
	rb.force = { 0.0f, 0.0f, 0.0f };

	// it's new position is last saved position
	trans.pos = state.spawnPos;
	resetCamera(playerID);
}

void SceneGolf::spawnPortal(const size_t playerID, const Vec3 pos)
{
	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[playerID];
	CRigidBody& rb = m_entityManager.getComponentVector<CRigidBody>()[playerID];
	CState& state = m_entityManager.getComponentVector<CState>()[playerID];

	// set state and rigidBody to default
	state.isGrounded = true;
	state.isCollided = false;
	state.isShot = true;

	rb.velocity = { 0.0f, 0.0f, 0.0f };
	rb.force = { 0.0f, 0.0f, 0.0f };

	// it's new position is last saved position
	trans.pos = pos;
	resetCamera(playerID);
}

void SceneGolf::resetCamera(const size_t playerID)
{
	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[playerID];
	// set the camera so it is same moves back 20.0f and up 5.0f in trans.forward direction
	camera.pos = trans.pos;
	camera.pos -= trans.forward * 20.0f;
	camera.pos += camera.up * 5.0f;

	// set the camera to be pointing entity
	camera.forward = trans.pos;
}