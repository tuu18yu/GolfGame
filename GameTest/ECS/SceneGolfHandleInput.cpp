#include "stdafx.h"
#include "SceneGolf.h"
#include "App/app.h"

void SceneGolf::sGetInput(const size_t playerID)
{
	CTransform& trans = m_entityManager.getComponentVector<CTransform>()[playerID];
	CInput& input = m_entityManager.getComponentVector<CInput>()[playerID];
	CState& state = m_entityManager.getComponentVector<CState>()[playerID];
	

	// Keyboard Control
	// Key pressed
	if (App::IsKeyPressed('W')) { input.up = true; }
	if (App::IsKeyPressed('A')) { input.left = true; }
	if (App::IsKeyPressed('S')) { input.down = true; }
	if (App::IsKeyPressed('D')) { input.right = true; }

	// since I cannot detect if space has been released or not, I introduce two booleans 
	if (state.isGrounded && !state.isShot && App::IsKeyPressed(VK_SPACE))
	{
		// m_isShotPressed checks if space is currently being pressed and used to detect whether it has been release
		m_isShotPressed = true; 
	}

	if (state.isGrounded && !state.isShot && (App::IsKeyPressed(VK_SHIFT) || App::IsKeyPressed(VK_RSHIFT)))
	{
		m_isPutterPressed = true;
	}


	// Key released
	if (!App::IsKeyPressed('W')) { input.up = false; }
	if (!App::IsKeyPressed('A')) { input.left = false; }
	if (!App::IsKeyPressed('S')) { input.down = false; }
	if (!App::IsKeyPressed('D')) { input.right = false; }
	if (m_isShotPressed && !App::IsKeyPressed(VK_SPACE))
	{
		m_isShotPressed = false;
		// then I can detect is space is released and actually shoot the ball
		m_isShotReleased = true;
		
	}
	if (m_isPutterPressed && (!App::IsKeyPressed(VK_SHIFT) && !App::IsKeyPressed(VK_RSHIFT)))
	{
		m_isPutter = !m_isPutter;
		m_isPutterPressed = false;
		if (!App::IsSoundPlaying("Data/switch.wav"))
		{
			App::PlaySoundW("Data/switch.wav");
		}
	}

	moveCamera(trans, input);
	getShotIntensity(input);
}

void SceneGolf::moveCamera(CTransform& trans, CInput& input)
{
	Vec3 forward = (trans.pos - camera.pos).normalize();
	Vec3 up = Vec3(0.0f, 1.0f, 0.0f); // Assuming world up is (0, 1, 0)
	Vec3 left = up.cross_product(forward).normalize();
	Vec3 right = left * -1.0f; // Right is the opposite of left
	// Move camera
	if (input.left)
	{
		camera.pos += right * 0.4f;
	}
	if (input.right)
	{
		camera.pos += left * 0.4f;
	}
	if (input.up)
	{
		//camera.pos.y += 0.4f;
		camera.pos += forward * 0.4f;
	}
	if (input.down)
	{
		//camera.pos.y -= 0.4f;
		camera.pos -= forward * 0.4f;
	}
}

void SceneGolf::getShotIntensity(CInput& input)
{
	if (m_isShotPressed)
	{
		if ((0.0f <= m_intensity) && (m_intensity < 1.0f))
		{
			m_intensity += m_intensityIncrease;
		}

		else
		{
			m_intensityIncrease = -m_intensityIncrease;
			m_intensity += m_intensityIncrease;
		}
	}

	if (m_isShotReleased)
	{
		input.shoot = true;
		m_isShotReleased = false;
	}
}

void SceneGolf::getMenuInput()
{
	if (App::IsKeyPressed('1'))
	{
		m_menu = false;
		m_isGamePlaying = true;
		m_isMultiplayer = false;

	}
	if (App::IsKeyPressed('2')) 
	{ 
		m_menu = false;
		m_isGamePlaying = true;
		m_isMultiplayer = true;

		// make the portal time longer as player needs to take turns
		redPortalTime = 20.0f;
		bluePortalTime = 25.0f;

	}
}

