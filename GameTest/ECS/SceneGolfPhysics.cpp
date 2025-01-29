#include "stdafx.h"
#include "SceneGolf.h"

void SceneGolf::sApplyForce(const float deltaTime, const size_t playerID)
{
	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[playerID];
	CInput& input = m_entityManager.getComponentVector<CInput>()[playerID];
	CRigidBody& rb = m_entityManager.getComponentVector<CRigidBody>()[playerID];
	CState& state = m_entityManager.getComponentVector<CState>()[playerID];

	// since in this game, collision between balls only happen on ground we can just apply the impulse of collision
	if (state.isCollided)
	{
		rb.force /= deltaTime; // this value carried on from collision detection
		state.isCollided = false; // since all the impulse of collision is exerted set back to 0
		return;
	}

	// Initially, no force is applied to the object
	rb.force = { 0.0f, 0.0f, 0.0f };
	// gravity is always applied
	rb.force += (m_gravity * rb.mass);


	// state 1: resting golf ball (on ground and velocity = 0)
	// also at state 1 turn ends
	if (state.isGrounded && rb.velocity.getLength() < velocityThreshold)
	{
		if (App::IsSoundPlaying("Data/rolling.wav"))
		{
			App::StopSound("Data/rolling.wav");
		}

		if (state.isOnMovingBlock != -1)
		{
			CRigidBody& rb2 = m_entityManager.getComponentVector<CRigidBody>()[state.isOnMovingBlock];
			trans.pos += (rb2.velocity * deltaTime);
		}

		// if it was shot before resting position then turn ends
		if (state.isShot && m_currentPlayer == playerID)
		{
			// turn ends when current player stops moving -> want to change when both player stops moving
			// added to component to check if both player has stopped moving
			//m_turnEnded = true;
			state.turnEnded = true;
			state.isShot = false;
		}

		if (m_isMultiplayer && m_currentPlayer != playerID)
		{
			// when it's multiplayer check if other player is also resting
			state.turnEnded = true;
		}

		// ensure velocity is 0
		rb.velocity = { 0.0f, 0.0f, 0.0f };
		// apply normal force
		rb.force -= (m_gravity * rb.mass);
		// set the last resting state as spawn point when it needs to respawn but cannot be moving block
		if (state.isOnMovingBlock == -1)
		{
			state.spawnPos = trans.pos;
		}


		// state 2: when user shoot the ball
		// note shooting only happens when the ball is resting position
		if (input.shoot)
		{
			// each time golf swing move is made, it's one move
			state.totalMoves += 1;
			m_toal_moves += 1;

			state.isShot = true; // ball is being shot
			input.shoot = false; // set to false since it's been shot

			// based on the type of club being used apply force in forward direction
			Vec3 forward = trans.pos - camera.pos;
			forward.y = 0.0f;
			forward = forward.normalize();
			trans.forward = forward;

			if (m_isPutter)
			{
				if (!App::IsSoundPlaying("Data/putt.wav"))
				{
					App::PlaySoundW("Data/putt.wav");
				}

				rb.force += forward * 290.0f * rb.mass * m_intensity;
			}
			else
			{
				if (!App::IsSoundPlaying("Data/driver.wav"))
				{
					App::PlaySoundW("Data/driver.wav");
				}


				// hitting with driver causes the ball to jump
				state.isGrounded = false;
				// 10 to 80
				float angle = 10.0f + m_intensity * (80.0f);
				float angleRad = angle * (PI / 180.0f); // Convert to radians

				// Adjust Y-component based on the angle
				forward.y = tan(angleRad); // Add height proportional to the angle
				forward = forward.normalize();

				// use the intensity to get the angle from 0 to 90 and adjust y
				rb.force += forward * 330.0f * rb.mass;
			}

			m_intensity = 0.0f; // set the m_intensity back to 0 as it's been used for shot
		}
	}

	// state 3: when ball is in air
	else if (!state.isGrounded && state.isShot)
	{
		// apply drag force
		//rb.force.z += (m_drag);

		// apply lift force
		//rb.force.y += m_lift;

		// do nothing as drag and lift is not necessary
		return;
	}

	// state 4: when ball is rolling on the ground
	else if (state.isGrounded && rb.velocity.getLength() >= velocityThreshold)
	{
		if (!App::IsSoundPlaying("Data/rolling.wav"))
		{
			App::PlaySoundW("Data/rolling.wav", true);
		}

		if (state.isOnMovingBlock != -1)
		{
			CRigidBody& rb2 = m_entityManager.getComponentVector<CRigidBody>()[state.isOnMovingBlock];
			trans.pos += (rb2.velocity * deltaTime);
		}

		// apply normal force
		rb.force -= (m_gravity * rb.mass);

		// apply kinetic friction coefficient of friction * mass
		rb.force -= rb.velocity.normalize() * rb.friction * rb.mass;

	}

	// state 5: not being shot but falling because of collision
	// gravity is being applied


}

void SceneGolf::sUpdatePosition(const float deltaTime, const size_t playerID)
{
	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[playerID];
	CInput& input = m_entityManager.getComponentVector<CInput>()[playerID];
	CRigidBody& rb = m_entityManager.getComponentVector<CRigidBody>()[playerID];
	CState& state = m_entityManager.getComponentVector<CState>()[playerID];
	CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[playerID];

	// save the previous position before changing current position
	trans.prevPos = trans.pos;

	// change in position = (v0 * dt) + 1/2 (a * dt^2)
	Vec3 a = rb.force / rb.mass;
	// v = v0 + a * dt
	rb.velocity += a * deltaTime;
	// change in position = (v0 * dt) + 1/2 (a * dt^2)
	trans.pos += (rb.velocity * deltaTime) + (a * (deltaTime * deltaTime)) / 2.0f;

	// falls below height threshold loses a life
	if (trans.pos.y < heightThreshold && state.isAlive)
	{
		
		if (!App::IsSoundPlaying("Data/dead.wav"))
		{
			App::PlaySoundW("Data/dead.wav");
		}

		state.totalLives -= 1;
		// set isAlive false will check at beginning of the loop if there's remaining life and set back to true
		state.isAlive = false;
		state.turnEnded = true; // also the turn ends at death

		//// if the player died is the player playing end the turn
		//if (m_currentPlayer == playerID)
		//{
		//	size_t otherPlayer = (playerID == m_pID) ? m_pID2 : m_pID;
		//	CState& state2 = m_entityManager.getComponentVector<CState>()[otherPlayer];
		//	state2.turnEnded = true;

		//	// since the dead player won't respawn until next next turn
		//	m_isOnePlayerScene = true;
		//}
		return;

	}

	// update position of the entity, bounding box, and the aabb in the node of BVH
	box.pos = trans.pos;
	m_entityManager.getBVH().UpdateEntityPosition(playerID, m_entityManager);
	
	if (m_isOnePlayerScene) 
	{
		// when not a multiplayer I don't want camera to be resetting every turn
		// just follow the y-axis
		camera.pos.y = trans.pos.y + 5.0f;
		camera.forward = trans.pos;
	}
}

void SceneGolf::sMoveBlocks(const float deltaTime, const size_t blockID)
{
	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[blockID];
	CRigidBody& rb = m_entityManager.getComponentVector<CRigidBody>()[blockID];
	CState& state = m_entityManager.getComponentVector<CState>()[blockID];
	CBoundingBox& box = m_entityManager.getComponentVector<CBoundingBox>()[blockID];

	// if postion is not between start and spawn position aka (destination) negate velocity
	// check the axis travelling on
	if (rb.velocity.x != 0.0f)
	{
		trans.pos.x += (rb.velocity.x * deltaTime);
		if (trans.pos.x < min(state.startPos.x, state.spawnPos.x) ||
			trans.pos.x > max(state.startPos.x, state.spawnPos.x))
		{
			rb.velocity.x = -rb.velocity.x; 
		}
	}
	else if (rb.velocity.y != 0.0f)
	{
		trans.pos.y += (rb.velocity.y * deltaTime);
		if (trans.pos.y < min(state.startPos.y, state.spawnPos.y) ||
			trans.pos.y > max(state.startPos.y, state.spawnPos.y))
		{
			rb.velocity.y = -rb.velocity.y; 
		}
	}
	else if (rb.velocity.z != 0.0f)
	{
		trans.pos.z += (rb.velocity.z * deltaTime);
		if (trans.pos.z < min(state.startPos.z, state.spawnPos.z) ||
			trans.pos.z > max(state.startPos.z, state.spawnPos.z))
		{
			rb.velocity.z = -rb.velocity.z;
		}
	}

	box.pos = trans.pos;
	m_entityManager.getBVH().UpdateEntityPosition(blockID, m_entityManager);
}