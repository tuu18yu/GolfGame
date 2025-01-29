#include "stdafx.h"
#include "SceneGolf.h"
#include "App/app.h"

void SceneGolf::displayIntensity()
{
	Square fillRectangle;
	float width = 600.0f;

	fillRectangle.fillColor = { 0.859f, 0.639f, 0.055f };

	fillRectangle.points[0] = { 212.0f, 50.0f, 0.0f };
	fillRectangle.points[1] = { 212.0f, 100.0f, 0.0f };
	fillRectangle.points[2] = { 212.0f + (width * m_intensity), 100.0f, 0.0f };
	fillRectangle.points[3] = { 212.0f + (width * m_intensity), 50.0f, 0.0f };

	
	renderer.Fill(fillRectangle);

	if (!m_isPutter)
	{
		App::DrawLine(212.0f + (width * 0.5f), 50.0f, 212.0f + (width * 0.5f), 100.0f);
	}
}

void SceneGolf::displayDriverType() const
{
	if (m_isPutter)
	{
		App::Print(40.0f, 50.0f, "Club Type: PUTTER");
	}
	else
	{
		App::Print(40.0f, 50.0f, "Club Type: DRIVER");
	}
}

void SceneGolf::displayPlayerStatus(const Vec2& pos, const size_t ID) const
{
	int totalLives = m_total_lives;
	int totalMoves = 0;


	if (ID != -1)
	{
		CState& state = m_entityManager.getComponentVector<CState>()[ID];
		totalLives = state.totalLives;
		totalMoves = state.totalMoves;
	}

	char text[50] = "Lives: ";
	for (int i = 0; i < m_total_lives; i++)
	{
		char c1 = 'O';

		if (totalLives - i > 0)
		{
			std::strncat(text, &c1, 1);
		}
	}

	if (totalLives < 0)
	{
		char c1 = 'X';
		std::strncat(text, &c1, 1);
	}


	char text2[50];
	std::sprintf(text2, "Moves: %d", totalMoves);

	if (ID == m_pID)
	{
		App::Print(pos.x, pos.y + 40, "Player 1");
	}
	else
	{
		App::Print(pos.x, pos.y + 40, "Player 2");
	}
	App::Print(pos.x, pos.y + 20, text);
	App::Print(pos.x, pos.y, text2);
}

void SceneGolf::displayMenu()
{
	// w = 1024, h = 768 
	float halfWidth = WINDOW_WIDTH / 2.0f;
	float halfHeight = WINDOW_HEIGHT / 2.0f;

	App::Print(halfWidth - 80.0f, halfHeight + 100, "2025 NEXT Golf");
	App::Print(halfWidth - 100.0f, halfHeight, "Single Player: Press 1");
	App::Print(halfWidth - 95.0f, halfHeight - 30, "Multi Player: Press 2");

}

void SceneGolf::displayResult()
{
	float halfWidth = WINDOW_WIDTH / 2.0f;
	float halfHeight = WINDOW_HEIGHT / 2.0f;

	if (!m_isMultiplayer)
	{
		CState& state = m_entityManager.getComponentVector<CState>()[m_currentPlayer];
		if (m_didWin)
		{
			App::Print(halfWidth - 80.0f, halfHeight + 100, "YOU WIN");
		}
		else
		{
			App::Print(halfWidth - 80.0f, halfHeight + 100, "YOU LOST");
		}
		char text[50];
		std::sprintf(text, "Total Moves: %d", state.totalMoves);
		App::Print(halfWidth - 10.0f, halfHeight + 60, text);
	}
	else
	{
		CState& state = m_entityManager.getComponentVector<CState>()[m_pID];
		CState& state2 = m_entityManager.getComponentVector<CState>()[m_pID2];
		if (m_didWin)
		{
			// whoever reaches the goal first wins
			if (m_currentPlayer == m_pID)
			{
				App::Print(halfWidth - 80.0f, halfHeight + 100, "Player1 WINS");
			}
			else
			{
				App::Print(halfWidth - 80.0f, halfHeight + 100, "Player2 WINS");
			}
		}
		else
		{
			// whoever died first loses, and as game ends when one player dies
			// find the player with less than 0 lives
			if (state.totalLives < 0)
			{
				App::Print(halfWidth - 80.0f, halfHeight + 100, "Player2 WINS");
			}
			else
			{
				App::Print(halfWidth - 80.0f, halfHeight + 100, "Player1 WINS");
			}
		}
		char text1[50];
		std::sprintf(text1, "Player1 Total Moves: %d", state.totalMoves);
		App::Print(halfWidth - 10.0f, halfHeight + 60, text1);

		char text2[50];
		std::sprintf(text2, "Player2 Total Moves: %d", state2.totalMoves);
		App::Print(halfWidth - 10.0f, halfHeight + 40, text2);
	}
}