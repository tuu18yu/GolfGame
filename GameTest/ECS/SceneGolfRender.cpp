#include "stdafx.h"
#include "SceneGolf.h"
#include "App/app.h"
#include <algorithm>

void SceneGolf::drawEntities() const
{
	std::vector<size_t> indicies;
	std::vector<size_t> visibleEntities;

	Frustum frustum;
	renderer.ExtractFrustum(frustum, camera);

	m_entityManager.getBVH().FrustumCulling(frustum, visibleEntities);

	//for (int i = 0; i < MAX_ENTITIES; i++)
	for (auto &i: visibleEntities)
	{
		CMesh cmesh = m_entityManager.getComponentVector<CMesh>()[i];
		CTransform trans = m_entityManager.getComponentVector<CTransform>()[i];
		std::string type = m_entityManager.getTag(i);
		if (cmesh.has && trans.has && type != "ball")
		{
			Vec3 forward = (camera.forward - camera.pos).normalize();
			CTransform currBallTrans = m_entityManager.getComponentVector<CTransform>()[m_currentPlayer];

			float dot = trans.pos.dot_product(forward);
			float dot2 = currBallTrans.pos.dot_product(forward);

			indicies.push_back(i);
			// only render entities that are further than the current ball
			// add some padding to draw some blocks that are slightly behind the camera
			/*if (dot + 1.0f >= dot2)
			{
				indicies.push_back(i);
			}*/
		}
	}

	// Sort triangle in order of z, z is ordered based on average z value
	// if z is the same sort type ball first -> changed to sorting by forward vector
	std::sort(indicies.begin(), indicies.end(), [&](size_t i1, size_t i2)
		{
			Vec3 forward = (camera.forward - camera.pos).normalize();

			CTransform cTrans1 = m_entityManager.getComponentVector<CTransform>()[i1];
			CTransform cTrans2 = m_entityManager.getComponentVector<CTransform>()[i2];

			float dot1 = cTrans1.pos.dot_product(forward);
			float dot2 = cTrans2.pos.dot_product(forward);

			/*if (cTrans1.pos.z == cTrans2.pos.z)
			{
				return cTrans1.pos.x > cTrans2.pos.x;;
			}

			return cTrans1.pos.z < cTrans2.pos.z;*/
			return dot1 > dot2;
		});


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

#ifdef _DEBUG
		renderer.Draw(camera, mesh, true);
#else
		renderer.Draw(camera, mesh);
#endif

	}
}

void SceneGolf::drawBalls() const
{

	if (m_isMultiplayer && m_pID2 != -1)
	{
		Vec3 forward = (camera.forward - camera.pos).normalize();
		size_t otherPlayer = (m_currentPlayer == m_pID) ? m_pID2 : m_pID;
		CTransform trans1 = m_entityManager.getComponentVector<CTransform>()[m_currentPlayer];
		CTransform trans2 = m_entityManager.getComponentVector<CTransform>()[otherPlayer];

		float dot1 = trans1.pos.dot_product(forward);
		float dot2 = trans2.pos.dot_product(forward);

		if (dot1 >= dot2 + 1.0f)
		{
			drawBall(m_currentPlayer);
			//drawBall(otherPlayer);
		}
		else
		{
			drawBall(otherPlayer);
			drawBall(m_currentPlayer);
		}
	}
	else
	{
		drawBall(m_pID);
	}
}

void SceneGolf::drawBall(const size_t ID) const
{
	CMesh cmesh = m_entityManager.getComponentVector<CMesh>()[ID];
	CState state = m_entityManager.getComponentVector<CState>()[ID];
	CTransform trans = m_entityManager.getComponentVector<CTransform>()[ID];

	Mesh mesh = meshes[cmesh.type];
	mesh.scale = trans.scale;
	mesh.pos = trans.pos;
	mesh.color = cmesh.color;
	mesh.fYaw = trans.fYaw;
	mesh.fRoll = trans.fRoll;
	mesh.fPitch = trans.fPitch;

#ifdef _DEBUG
	renderer.Draw(camera, mesh, true);
#else
	if (state.isAlive)
	{
		renderer.Draw(camera, mesh);
	}
#endif
}