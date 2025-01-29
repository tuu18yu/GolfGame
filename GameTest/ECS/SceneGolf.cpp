#include "stdafx.h"
#include "SceneGolf.h"

void SceneGolf::Init()
{
	m_toal_moves = 0;

	m_pID = -1;
	m_pID2 = -1;
	m_currentPlayer = -1;

	m_isPutterPressed = false;
	m_isPutter = false;

	m_isShotPressed = false;
	m_isShotReleased = false;

	m_isMultiplayer = false;
	m_isOnePlayerScene = false;
	m_isGamePlaying = false;
	m_menu = true;
	m_didWin = false;

	createMap("./Data/map_final.txt");

	// set camera
	resetCamera(m_pID);
	m_currentPlayer = m_pID;
	m_isOnePlayerScene = true;

	m_redPortalCoolDown.reset(0.0f);
	m_bluePortalCoolDown.reset(0.0f);

	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[m_pID];
	m_particleGoal.setPosition({ 15.0f, 3.0f, 3.0f }); // location of the goal
	m_particleGoal.resetParticles();

}
void SceneGolf::Render()
{


	if (m_menu)
	{
		displayMenu();
	}

	else if (m_isGamePlaying)
	{
#ifdef _DEBUG
		// draw aabb tree
		for (auto& node : m_entityManager.getBVH().nodes)
		{
			renderer.Draw(camera, node.aabb);
		}
#endif
		renderer.Draw(camera, m_particleGoal);
		drawEntities();
		renderer.Draw(camera, m_particleHit);
		drawBalls();
		displayIntensity();
		displayDriverType();
		displayPlayerStatus({ 40.0f, 700.0f }, m_pID);
		if (m_isMultiplayer) { displayPlayerStatus({ 800.0f, 700.0f }, m_pID2); }
	}

	else if (!m_menu && !m_isGamePlaying)
	{
		renderer.Draw(camera, m_particleGoal);
		drawEntities();
		displayResult();
	}
}

void SceneGolf::Update(const float deltaTime)
{
	
	if (m_menu)
	{
		float dt = deltaTime * 0.001f;
		getMenuInput();
	}

	else if (m_isGamePlaying)
	{
		float dt = deltaTime * 0.001f;
		// set maximum dt, else when it's laggy and dt is big collision will not be detected and tunneling
		// this is because object moves too much on each update loop it doesn't collide
		dt = min(deltaTime, maxDeltaTime);
		// if player is dead and no morre lives end the game
		CState& state = m_entityManager.getComponentVector<CState>()[m_currentPlayer];
		if (!state.isAlive && state.totalLives < 0)
		{
			m_isGamePlaying = false;
		}

		// when turn ends and multiplayer game change the current player
		if (m_isMultiplayer)
		{
			// if player 2 has not yet created
			if (m_pID2 == -1)
			{
				// if player finished turn create and set the current player to player2
				CState& state = m_entityManager.getComponentVector<CState>()[m_currentPlayer];
				if (state.turnEnded)
				{
					Vec3 scale(1.0f, 1.0f, 1.0f);
					float mass = 0.45f;
					m_pID2 = createBall(m_startPostion, scale / 3.0f, m_ball2Color);
					// set to player 2
					m_currentPlayer = m_pID2;

					if (state.isAlive)
					{
						m_isOnePlayerScene = false;
						state.turnEnded = false;
					}
					resetCamera(m_currentPlayer);
				}
			}
			// both player exists in the memory pool
			else
			{	
				CState& state = m_entityManager.getComponentVector<CState>()[m_pID];
				CState& state2 = m_entityManager.getComponentVector<CState>()[m_pID2];
				// if both of the balls have finished moving
				if (state.turnEnded && state2.turnEnded)
				{
					size_t otherPlayer = (m_currentPlayer == m_pID) ? m_pID2 : m_pID;
					CState& state = m_entityManager.getComponentVector<CState>()[m_currentPlayer];
					CState& state2 = m_entityManager.getComponentVector<CState>()[otherPlayer];

					// if the current player is still alive, there are going to be 2 balls in the scene
					m_isOnePlayerScene = state.isAlive ? false : true;
					
					// if the other player is not alive check for lives and respawn
					if (!state2.isAlive && state2.totalLives >= 0)
					{
						// respawn the other player
						respawn(otherPlayer);
						state2.isAlive = true;
					}

					// then we can safely change the turn to other player
					if (state2.isAlive)
					{
						m_currentPlayer = (m_currentPlayer == m_pID) ? m_pID2 : m_pID;
						resetCamera(m_currentPlayer);

						// reset the turns
						state.turnEnded = false;
						state2.turnEnded = false;
					}
					// if the other player is not alive end the game
					else
					{
						m_isGamePlaying = false;
					}
				}
			}

		}
		else // when game is not multiplayer
		{
			if (!state.isAlive && state.totalLives >= 0)
			{
				respawn(m_currentPlayer);
				state.isAlive = true;
				state.turnEnded = false;
			}
		}

	
		if (!m_isOnePlayerScene)
		{
			size_t otherPlayer = (m_currentPlayer == m_pID) ? m_pID2 : m_pID;
			// get the user input for moving the current player
			sGetInput(m_currentPlayer);

			// apply force for both balls
			sApplyForce(dt, m_currentPlayer);
			// since user input doesn't impact, only force being exerted is from collision and gravity (when falls after collision)
			sApplyForce(dt, otherPlayer);

			for (int i = 0; i < MAX_ENTITIES; i++)
			{
				std::string type = m_entityManager.getTag(i);
				if (type == "movingBlock")
				{
					sMoveBlocks(dt, i);
				}
			}

			// update the position based on applied force
			sUpdatePosition(dt, m_currentPlayer);
			sUpdatePosition(dt, otherPlayer);

			// check all the entities that had been collided with the current player
			// including the stationary other ball
			sBallCollision(m_currentPlayer);
			sBallCollision(otherPlayer);
		}
		else
		{
			if (m_isMultiplayer && m_pID2 != -1)
			{
				size_t otherPlayer = (m_currentPlayer == m_pID) ? m_pID2 : m_pID;
				CState& state = m_entityManager.getComponentVector<CState>()[otherPlayer];
				state.turnEnded = true;

			}
			sGetInput(m_currentPlayer);
			sApplyForce(dt, m_currentPlayer);
			for (int i = 0; i < MAX_ENTITIES; i++)
			{
				std::string type = m_entityManager.getTag(i);
				if (type == "movingBlock")
				{
					sMoveBlocks(dt, i);

				}
			}
			sUpdatePosition(dt, m_currentPlayer);
			sBallCollision(m_currentPlayer);
		}

		// set up cooldowns and update particle
		m_redPortalCoolDown.update(dt);
		m_bluePortalCoolDown.update(dt);
		m_particleGoal.update(dt);
		m_particleHit.update(dt);
				
	}
}

void SceneGolf::Shutdown()
{

}