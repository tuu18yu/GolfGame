#include "stdafx.h"
#include "SceneGolf.h"

void SceneGolf::sBallCollision(const size_t playerID)
{
	#ifdef _DEBUG
	for (size_t i = 0; i < MAX_ENTITIES; i++)
	{
			CMesh& cmesh = m_entityManager.getComponentVector<CMesh>()[i];
			if ("grass" == m_entityManager.getTag(i))
			{
				cmesh.color = m_grassColor;
			}

			else if ("sand" == m_entityManager.getTag(i))
			{
				cmesh.color = m_sandColor;
			}	
	}
	#endif

	std::vector<size_t> indicies;

	// get indices of all the objects that collide with player
	CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[playerID];
	m_entityManager.getBVH().Query(box.nodeID, indicies);

	// if no block is being collided with the player, it's not grounded
	CState& state = m_entityManager.getComponentVector<CState>()[playerID];
	state.isOnMovingBlock = -1;
	if (indicies.size() == 0) { state.isGrounded = false; }


	// resolve collision for all collided objects
	for (auto& i : indicies)
	{
		#ifdef _DEBUG
			CMesh& cmesh = m_entityManager.getComponentVector<CMesh>()[i];
			cmesh.color = m_collideColor;
		#endif
		std::string type = m_entityManager.getTag(i);
		if (type == "ball")
		{
			if (playerID != m_currentPlayer)
			{
				// to avoid double checking
				return;
			}

			ballCollision(playerID, i);
		}
		else if (type == "goal")
		{
			m_isGamePlaying = false;
			m_didWin = true;

			if (!App::IsSoundPlaying("Data/win.wav"))
			{
				App::PlaySoundW("Data/win.wav");
			}
		}
		else
		{
			if (type == "redPortal")
			{
				CState& state = m_entityManager.getComponentVector<CState>()[i];
				if (m_redPortalCoolDown.isReady())
				{
					if (!App::IsSoundPlaying("Data/teleport.wav"))
					{
						App::PlaySoundW("Data/teleport.wav");
					}
					spawnPortal(playerID, state.spawnPos);
					m_redPortalCoolDown.reset(redPortalTime);
				}
				else
				{
					levelCollision(playerID, i);
				}
			}

			else if (type == "bluePortal")
			{
				CState& state = m_entityManager.getComponentVector<CState>()[i];
				if (m_bluePortalCoolDown.isReady())
				{
					if (!App::IsSoundPlaying("Data/teleport.wav"))
					{
						App::PlaySoundW("Data/teleport.wav");
					}
					spawnPortal(playerID, state.spawnPos);
					m_bluePortalCoolDown.reset(bluePortalTime);
				}
				else
				{
					levelCollision(playerID, i);
				}
			}
			else if (type == "movingBlock")
			{
				CState& state = m_entityManager.getComponentVector<CState>()[playerID];
				//CRigidBody& rb = m_entityManager.getComponentVector<CRigidBody>()[playerID];
				//rb.velocity = { 0.0f, 0.0f, 0.0f };
				state.isOnMovingBlock = i;
				levelCollision(playerID, i);
			}
			else
			{
				levelCollision(playerID, i);
			}
		}
	}
}

void SceneGolf::ballCollision(size_t ID1, size_t ID2)
{
	if (!App::IsSoundPlaying("Data/ball_hit.wav"))
	{
		App::PlaySoundW("Data/ball_hit.wav");
	}

	CBoundingBox& box1 = m_entityManager.getComponentVector<CBoundingBox>()[ID1];
	CBoundingBox& box2 = m_entityManager.getComponentVector<CBoundingBox>()[ID2];

	CTransform& trans1 = m_entityManager.getComponentVector<CTransform>()[ID1];
	CTransform& trans2 = m_entityManager.getComponentVector<CTransform>()[ID2];

	CRigidBody& rb1 = m_entityManager.getComponentVector<CRigidBody>()[ID1];
	CRigidBody& rb2 = m_entityManager.getComponentVector<CRigidBody>()[ID2];

	CState& state1 = m_entityManager.getComponentVector<CState>()[ID1];
	CState& state2 = m_entityManager.getComponentVector<CState>()[ID2];

	float overlapX = min(box1.max_corner().x, box2.max_corner().x) - max(box1.min_corner().x, box2.min_corner().x);
	float overlapY = min(box1.max_corner().y, box2.max_corner().y) - max(box1.min_corner().y, box2.min_corner().y);
	float overlapZ = min(box1.max_corner().z, box2.max_corner().z) - max(box1.min_corner().z, box2.min_corner().z);

	// add back the smallest overlap so objects don't overlap
	if (overlapX < overlapY && overlapX < overlapZ)
	{

		if (box1.min_corner().x < box2.min_corner().x)
		{
			trans1.pos.x -= overlapX;
		}
		else
		{
			trans1.pos.x += overlapX;
		}
	}
	else if (overlapY < overlapZ)
	{
		if (box1.min_corner().y < box2.min_corner().y)
		{
			trans1.pos.y -= overlapY;
		}
		else
		{
			trans1.pos.y += overlapY;
		}
	}
	else
	{
		if (box1.min_corner().z < box2.min_corner().z)
		{
			trans1.pos.z -= overlapZ;
		}
		else
		{
			trans1.pos.z += overlapZ;
		}

	}
	// update the new positon in the memorypool and the tree
	box1.pos = trans1.pos;
	m_entityManager.getBVH().UpdateEntityPosition(ID1, m_entityManager);

	// set the state to indicate they have collided	
	state1.isCollided = true;
	state2.isCollided = true;

	//since it is moving set the turn ended to false until finished moving
	state2.turnEnded = false;

	// note: always ball1 is moving and ball2 is staionary	
	Vec3 collisionNormal = (trans2.pos - trans1.pos).normalize(); // Collision axis

	// decompose the moving ball's velocity 
	Vec3 v1_parallel = collisionNormal * rb1.velocity.dot_product(collisionNormal);
	Vec3 v1_perpendicular = rb1.velocity - v1_parallel;

	// assume rb2 is initially stationary
	Vec3 v2_parallel = Vec3(0.0f, 0.0f, 0.0f);

	// update parallel velocities using 1D elastic collision formulas
	Vec3 v1_parallel_new = collisionNormal * ((v1_parallel.getLength() * (rb1.mass - rb2.mass)) / (rb1.mass + rb2.mass));
	Vec3 v2_parallel_new = collisionNormal * ((2.0f * v1_parallel.getLength() * rb1.mass) / (rb1.mass + rb2.mass));

	// Change in velocity
	Vec3 delta_v1 = v1_parallel_new - v1_parallel;
	Vec3 delta_v2 = v2_parallel_new - v2_parallel;

	// Compute forces
	rb1.force += (delta_v1 * rb1.mass);
	rb2.force += (delta_v2 * rb2.mass);
}



void SceneGolf::levelCollision(size_t ID1, size_t ID2)
{

	CBoundingBox& box1 = m_entityManager.getComponentVector<CBoundingBox>()[ID1];
	CBoundingBox& box2 = m_entityManager.getComponentVector<CBoundingBox>()[ID2];

	CTransform& trans1 = m_entityManager.getComponentVector<CTransform>()[ID1];

	CRigidBody& rb1 = m_entityManager.getComponentVector<CRigidBody>()[ID1];
	CRigidBody& rb2 = m_entityManager.getComponentVector<CRigidBody>()[ID2];

	CState& state = m_entityManager.getComponentVector<CState>()[ID1];


	// overlap in each axis
	float overlapX = min(box1.max_corner().x, box2.max_corner().x) - max(box1.min_corner().x, box2.min_corner().x);
	float overlapY = min(box1.max_corner().y, box2.max_corner().y) - max(box1.min_corner().y, box2.min_corner().y);
	float overlapZ = min(box1.max_corner().z, box2.max_corner().z) - max(box1.min_corner().z, box2.min_corner().z);
	
	// add back the smallest overlap so objects don't overlap
	if (overlapX < overlapY && overlapX < overlapZ) // x axis
	{
		if (box1.min_corner().x < box2.min_corner().x)
		{
			trans1.pos.x -= overlapX;
		}
		else
		{
			trans1.pos.x += overlapX;
		}

		if (!App::IsSoundPlaying("Data/wall_hit.wav"))
		{
			App::PlaySoundW("Data/wall_hit.wav");
		}

		rb1.velocity.x *= -0.25f; // ball bounces of the wall but slower as the wall absorbed some of the force

		CMesh& mesh = m_entityManager.getComponentVector<CMesh>()[ID2];
		m_particleHit.setColor(mesh.color);
		m_particleHit.setPosition(trans1.pos);
		m_particleHit.setVelocity(rb1.velocity); // location of the goal
		m_particleHit.setDuration(2);
		m_particleHit.resetParticles(50);
		m_particleHit.axis = 0;
	}
	else if (overlapY < overlapZ)
	{
		if (box1.min_corner().y < box2.min_corner().y) // below the block
		{
			trans1.pos.y -= overlapY;
			rb1.velocity.y *= -0.25f; // ball bounces of the block in opposite direction
		}
		else // above the floor, block
		{
			trans1.pos.y += overlapY;

			state.isGrounded = true; // touching the ground

			// save the friction of the material to rb component for calculating forces
			// note: in this game friction of static object is coeff of friction of material
			// friction of kinetic object is the friction being applies
			rb1.friction = rb2.has ? rb2.friction : 0.0f;

			// when hits the ground by the driver
			if (state.isShot && !m_isPutter)
			{
				// when lands the ground make velocity small for better game control
				// else rolls on the ground too much after jumping
				

				rb1.velocity *= 0.8f;
			}


		}
		rb1.velocity.y = 0.0f;
	}
	else // z axis
	{
		if (box1.min_corner().z < box2.min_corner().z)
		{
			trans1.pos.z -= overlapZ;
		}
		else
		{
			trans1.pos.z += overlapZ;
		}

		if (!App::IsSoundPlaying("Data/wall_hit.wav"))
		{
			App::PlaySoundW("Data/wall_hit.wav");
		}


		rb1.velocity.z *= -0.25f; // ball bounces of the wall but slower as the wall absorbed some of the force
		CMesh& mesh = m_entityManager.getComponentVector<CMesh>()[ID2];
		m_particleHit.setColor(mesh.color);
		m_particleHit.setPosition(trans1.pos);
		m_particleHit.setVelocity(rb1.velocity); // location of the goal
		m_particleHit.setDuration(2);
		m_particleHit.resetParticles(50);
		m_particleHit.axis = 2; // z
	}
	

	box1.pos = trans1.pos;
	m_entityManager.getBVH().UpdateEntityPosition(ID1, m_entityManager);
}