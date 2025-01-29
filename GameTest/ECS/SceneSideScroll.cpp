#include "stdafx.h"

#include "SceneSideScroll.h"
#include "../App/app.h"

#include <algorithm>
#include <cmath>

SceneSideScroll::SceneSideScroll()
{
}

void SceneSideScroll::Init()
{
	createMap("Data/platform2.txt");

	CTransform& cTrans = m_entityManager.getComponentVector<CTransform>()[m_pID];
	camera.forward = cTrans.pos;

}


void SceneSideScroll::createMap(std::string filename)
{
	float zVal = -50.0f;

	int row = 0, col = 0;
	int currCol = 0;

	//std::vector<size_t> IDs;

	std::ifstream file(filename);

	while (!file.eof())
	{
		char line[128];
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
					createBlock({ 0.0f + i, static_cast<float>(col - currCol), zVal }, { 1.0f, 1.0f, 1.0f }, m_blockColor);
				}

				else if (line[i] == 'o')
				{
					createBlock({ 0.0f + i, static_cast<float>(col - currCol), zVal }, { 1.0f, 1.0f, 1.0f }, m_obstacleColor, true);
				}

				// create player
				else if (line[i] == 'p')
				{
					Vec3 playerPos = { 0.0f + i, static_cast<float>(col - currCol), zVal };
					Vec3 playerScale = { 0.45f, 0.45f, 0.45f };
					createPlayer(playerPos, playerScale, 80.0f);
				}

				else if (line[i] == 't')
				{
					createTeapot({ 0.0f + i, static_cast<float>(col - currCol) - 0.45f, zVal }, { 0.15f, 0.15f, 0.15f }, m_teapotColor);
				}

			}

			currCol++;
		}
	}
	//m_entityManager.getBVH().Build(IDs, m_entityManager);
}

void SceneSideScroll::createBlock(Vec3 pos, Vec3 scale, Vec3 color, bool isObstacle)
{
	size_t ID = !isObstacle ? m_entityManager.addEntity("block") : m_entityManager.addEntity("obstacle");
	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[ID];
	trans.has = true;
	trans.pos = pos;
	trans.scale = scale;
	trans.fPitch = 10.f;

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

	/*if (isObstacle)
	{
		CDamage& damage = m_entityManager.getComponentVector<CDamage>()[ID];
		damage.has = true;
		damage.amount = 1;
	}*/

	m_entityManager.addEntityToBVH(ID);
}

void SceneSideScroll::createPlayer(Vec3 pos, Vec3 scale, float mass)
{
	m_pID = m_entityManager.addEntity("player");

	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[m_pID];
	trans.has = true;
	trans.pos = pos;
	trans.scale = scale;

	CMesh& mesh = m_entityManager.getComponentVector<CMesh>()[m_pID];
	mesh.has = true;
	mesh.type = CAPSULE;
	mesh.color = { 0.467f, 0.68f, 0.261f };

	CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[m_pID];
	box.has = true;
	box.pos = pos;
	box.scale = scale;
	box.setMaxCorner(meshes[CAPSULE].max_corner);
	box.setMinCorner(meshes[CAPSULE].min_corner);

	CInput& input = m_entityManager.getComponentVector<CInput>()[m_pID];
	input.has = true;

	CRigidBody& cRB = m_entityManager.getComponentVector<CRigidBody>()[m_pID];
	cRB.has = true;
	cRB.mass = mass;
	cRB.velocity = { 0.0f, 0.0f, 0.0f };

	CState& cState = m_entityManager.getComponentVector<CState>()[m_pID];
	cState.has = true;
	cState.isGrounded = false;
	cState.isDashing = false;
	cState.movedRight = true;
	cState.isAlive = true;
	cState.respawnCoolDown.reset(RESPAWN_COOLDOWN);
	cState.totalCollected = 0;

	m_entityManager.addEntityToBVH(m_pID);
}

void SceneSideScroll::createTeapot(Vec3 pos, Vec3 scale, Vec3 color)
{
	size_t ID = m_entityManager.addEntity("teapot");
	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[ID];
	trans.has = true;
	trans.pos = pos;
	trans.scale = scale;
	trans.fPitch = 5.0f;

	CMesh& mesh = m_entityManager.getComponentVector<CMesh>()[ID];
	mesh.has = true;
	mesh.type = TEAPOT;
	mesh.color = color;

	CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[ID];
	box.has = true;
	box.pos = pos;
	box.scale = scale;
	box.setMaxCorner(meshes[TEAPOT].max_corner);
	box.setMinCorner(meshes[TEAPOT].min_corner);


	m_entityManager.addEntityToBVH(ID);

}

void SceneSideScroll::Render()
{
	
	for (size_t i = 0; i < MAX_ENTITIES; i++)
	{
		if (i != m_pID) 
		{
			CMesh& cmesh = m_entityManager.getComponentVector<CMesh>()[i];
			CTransform& cTrans = m_entityManager.getComponentVector<CTransform>()[i];
			CBoundingBox& cBox = m_entityManager.getComponentVector<CBoundingBox>()[i];
			if (cmesh.has && cTrans.has && cBox.has)
			{

				Mesh mesh = meshes[cmesh.type];
				mesh.scale = cTrans.scale;
				mesh.pos = cTrans.pos;
				mesh.color = cmesh.color;
				mesh.fYaw = cTrans.fYaw;
				mesh.fRoll = cTrans.fRoll;
				mesh.fPitch = cTrans.fPitch;

				renderer.Draw(camera, mesh, false);
			}
		}
	}

	// draw player last
	if (m_entityManager.getActiveVector()[m_pID])
	{
		CMesh& cmesh = m_entityManager.getComponentVector<CMesh>()[m_pID];
		CTransform& cTrans = m_entityManager.getComponentVector<CTransform>()[m_pID];
		CBoundingBox& cBox = m_entityManager.getComponentVector<CBoundingBox>()[m_pID];

		Mesh mesh = meshes[cmesh.type];
		mesh.scale = cTrans.scale;
		mesh.pos = cTrans.pos;
		mesh.color = cmesh.color;
		mesh.fYaw = cTrans.fYaw;
		mesh.fRoll = cTrans.fRoll;
		mesh.fPitch = cTrans.fPitch;

		renderer.Draw(camera, mesh, false);
	}

	CState& state = m_entityManager.getComponentVector<CState>()[m_pID];
	if (state.totalCollected == TOTAL_TEAPOTS)
	{

		App::Print(GLUT_SCREEN_WIDTH / 2, GLUT_SCREEN_WIDTH / 2, "YOU WIN!! PRESS 'R' TO REPLAY");
	}
	// draw aabb tree
	for (auto& node : m_entityManager.getBVH().nodes)
	{
		renderer.Draw(camera, node.aabb);
	}
}

void SceneSideScroll::Update(const float deltaTime)
{
	// Note: delta time is 16.67 so I am * 0.001 to make more realistic
	float newDT = deltaTime * 0.0003f;

	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[m_pID];
	CRigidBody& rb = m_entityManager.getComponentVector<CRigidBody>()[m_pID];
	CState& state = m_entityManager.getComponentVector<CState>()[m_pID];
	CInput& input = m_entityManager.getComponentVector<CInput>()[m_pID];

	sInput(input, state);
	if (state.totalCollected == TOTAL_TEAPOTS)
	{
		if (input.replay)
		{
			sRevive();
		}
	}
	else if (state.isAlive)
	{
		// update cool down
		state.dashCoolDown.update(newDT);

		sUpdateVelocity(rb, input, trans, state, newDT);
		sUpdatePosition(rb, trans, state, newDT);
		sPlayerCollision();
		sRayCast(trans);
	}
	else
	{
		state.respawnCoolDown.update(newDT);
		if (state.respawnCoolDown.isReady())
		{
			sRevive();
		}
	}
}


void SceneSideScroll::sInput(CInput& input, CState& state)
{
	// Keyboard Contol

	// Key pressed
	if ((App::IsKeyPressed(VK_SPACE) || App::IsKeyPressed('W')) && state.isGrounded) { input.jump = true; }
	if (App::IsKeyPressed('D')) { input.left = true; }
	if (App::IsKeyPressed('A')) { input.right = true; }
	if ((App::IsKeyPressed(VK_SHIFT) || App::IsKeyPressed(VK_RSHIFT)) && !state.isDashing) { input.dash = true; }
	if (App::IsKeyPressed('R')) { input.replay = true; }

	// Key released
	if (!App::IsKeyPressed('D')) { input.left = false; }
	if (!App::IsKeyPressed('A')) { input.right = false; }
	if ((!App::IsKeyPressed(VK_SPACE) && !App::IsKeyPressed('W')) || !state.isGrounded) { input.jump = false; }
	if ((!App::IsKeyPressed(VK_SHIFT) && !App::IsKeyPressed(VK_RSHIFT)) || state.isDashing) { input.dash = false; } // cannot dash is player is currently dashing
	if (!App::IsKeyPressed('R')) { input.replay = false; }

	// Move camera
	if (App::IsKeyPressed(VK_LEFT))
	{
		camera.pos.x += 0.1f;
	}
	if (App::IsKeyPressed(VK_RIGHT))
	{
		camera.pos.x -= 0.1f;
	}
}

void SceneSideScroll::sUpdateVelocity(CRigidBody& rb, CInput& input, CTransform& trans, CState& state, const float deltaTime)
{
	// force on x-axis is only applied each frame when user inputs, so reset
	// cannot move left or right when dashing
	if (!state.isDashing)
	{
		rb.velocity.x = 0.0f; // reset the velocity to 0
		if (input.right)
		{
			//rb.force.x -= MOVE_SPEED * rb.mass;
			//camera.pos.x += 0.0005f;
			rb.velocity.x -= MOVE_SPEED;
			state.movedRight = true; // need this to check whether to move left or right when player dashes
		}
		if (input.left)
		{
			//rb.force.x += MOVE_SPEED * rb.mass;
			//camera.pos.x -= 0.0005f;
			rb.velocity.x += MOVE_SPEED;
			state.movedRight = false;
		}

		if (input.dash && state.dashCoolDown.isReady())
		{
			rb.velocity.x = !state.movedRight ? DASH : -DASH;
			state.isDashing = true;
			trans.beforeDash = trans.pos.x;
			state.dashCoolDown.reset(DASH_COOLDOWN);
		}
	}
	else
	{
		// if dashing add drag to player
		rb.velocity.x += !state.movedRight ? DRAG * deltaTime : -DRAG * deltaTime;
	}


	// apply gravity when not on the ground
	// when dashing pause the gravity
	if (!state.isGrounded && !state.isDashing) 
	{
		rb.velocity.y += GRAVITY * deltaTime;
	}
	else {
		rb.velocity.y = 0.0f; // Reset vertical velocity when on the ground
	}

	// jump only when rigid body is on the ground
	if (input.jump)
	{
		rb.velocity.y = JUMP_FORCE;
		trans.beforeJump = trans.pos.y;
		state.isGrounded = false; // Leave the ground
		input.jump = false;       // Reset jump after applying
	}

}

void SceneSideScroll::sUpdatePosition(CRigidBody& rb, CTransform& trans, CState& state, const float deltaTime)
{
	trans.prevPos = trans.pos;
	trans.pos += rb.velocity * deltaTime;

	if (state.isDashing)
	{
		// can't fall when dashing
		trans.pos.y = trans.prevPos.y;
	
		// cap the dash distance
		if (std::abs(trans.pos.x - trans.beforeDash) > 1.2f)
		{
			rb.velocity.x = 0.0f;
			state.isDashing = false;
		}

	}
	
	//cap the jump distance so it doesn't jump higher and drops
	if (!state.isGrounded && (trans.pos.y - trans.beforeJump) > 1.2f)
	{
		rb.velocity.y = -1.0f;
	}

	// if player falls
	if (trans.pos.y < -10.0f)
	{
		state.isAlive = false;
	}

	// Adjust Camera position accordingly
	// camera is behind player, to give perspective
	camera.forward = trans.pos;
	camera.pos.x = trans.pos.x;

	// Move the bounding box in the node as well
	CBoundingBox& cBox = m_entityManager.getComponentVector<CBoundingBox>()[m_pID];
	cBox.pos = trans.pos;
	m_entityManager.getBVH().UpdateEntityPosition(m_pID, m_entityManager);
}


void SceneSideScroll::sPlayerCollision()
{
	//for (size_t i = 0; i < MAX_ENTITIES; i++)
	//{
	//	//if ("block" == m_entityManager.getTag(i))
	//	{
	//		CMesh& cmesh = m_entityManager.getComponentVector<CMesh>()[i];
	//		cmesh.color = m_blockColor;
	//	}
	//}

	std::vector<size_t> indicies;
	// get index of all the objects that collide with player
	CBoundingBox& cBox = m_entityManager.getComponentVector<CBoundingBox>()[m_pID];
	CState& state = m_entityManager.getComponentVector<CState>()[m_pID];

	m_entityManager.getBVH().Query(cBox.nodeID, indicies);

	// if no block is being touched not on the ground
	if (indicies.size() == 0) { state.isGrounded = false; }

	for (auto& i : indicies)
	{
		/*CMesh& cmesh = m_entityManager.getComponentVector<CMesh>()[i];
		cmesh.color = m_collideColor;*/

		if (m_entityManager.getTag(i) == "obstacle")
		{
			state.isAlive = false;
		}

		else if (m_entityManager.getTag(i) == "teapot")
		{
			state.totalCollected += 1;
			m_entityManager.destoryEntity(i);
		}
		else
		{
			// Collision resolution
			sResolveCollision(m_pID, i);
		}
	}

}

void SceneSideScroll::sRayCast(CTransform& trans)
{
	size_t ID = -1;
	float t = 0.0f;
	Ray ray(trans.pos, { 0.0f, 0.0f, 1.0f });

	m_entityManager.getBVH().RayCast(ray, ID, t, m_pID);

	CMesh& cmesh = m_entityManager.getComponentVector<CMesh>()[ID];
	cmesh.color = m_collideColor;
}

void SceneSideScroll::sResolveCollision(size_t ID1, size_t ID2)
{
	CBoundingBox& box1 = m_entityManager.getComponentVector<CBoundingBox>()[ID1];
	CBoundingBox& box2 = m_entityManager.getComponentVector<CBoundingBox>()[ID2];

	CTransform& trans1 = m_entityManager.getComponentVector<CTransform>()[ID1];
	CTransform& trans2 = m_entityManager.getComponentVector<CTransform>()[ID2];

	CRigidBody& rb = m_entityManager.getComponentVector<CRigidBody>()[ID1];
	CState & state = m_entityManager.getComponentVector<CState>()[ID1];
	
	// Issue: when block is being collided while in air, and overlap in Y axis it pushes the object up

	// Calculate overlap along each axis
	float overlapX = min(box1.max_corner().x, box2.max_corner().x) - max(box1.min_corner().x, box2.min_corner().x);
	float overlapY = min(box1.max_corner().y, box2.max_corner().y) - max(box1.min_corner().y, box2.min_corner().y);


	// Resolve overlap based on the smallest penetration axis
	// Note that we only need to adjust on one side, pushing the player
	if (overlapX < overlapY)
	{
		// Horizontal collision

		// left of the box
		if (box1.min_corner().x < box2.min_corner().x)
		{
			trans1.pos.x -= overlapX;
			rb.velocity.x = 0.0f;
			state.isDashing = false;
		}
		// right of the box
		else
		{
			trans1.pos.x += overlapX;
			rb.velocity.x = 0.0f;
			state.isDashing = false;
		}

		// Reverse velocity on collision axis
		//RB1.velocity.x = -RB1.velocity.x * 0.5f; // Dampen velocity slightly
	}
	else
	{
		// Vertical collision

		// below the box collided
		if (box1.min_corner().y < box2.min_corner().y)
		{
			trans1.pos.y -= overlapY;
			// stop moving up velocity when box is blocking the jump
			rb.velocity.y = 0.0f;
		}

		// above the box collided
		else
		{
			trans1.pos.y += overlapY;
			state.isGrounded = true;
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

void SceneSideScroll::sRevive()
{
	m_entityManager.clear();
	createMap("Data/platform2.txt");

	CTransform& cTrans = m_entityManager.getComponentVector<CTransform>()[m_pID];
	camera.forward = cTrans.pos;
}

void SceneSideScroll::Shutdown() {}