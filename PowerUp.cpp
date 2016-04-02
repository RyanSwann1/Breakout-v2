#include "PowerUp.h"
#include "Game.h"
#include "Entity.h"



PowerUp::PowerUp()
{
	m_movementSpeed = 3;
}


PowerUp::~PowerUp()
{
}

bool PowerUp::collision(const SDL_Rect& powerUpPos, const SDL_Rect& entityPos)
{
	//If collision
	if (powerUpPos.x + powerUpPos.w > entityPos.x && entityPos.x < entityPos.x + entityPos.w &&
		powerUpPos.y + powerUpPos.h > entityPos.y && powerUpPos.y < entityPos.y + entityPos.h) {
		return true;
	}

	return false;
}

void PowerUp::movement(const Game & game, std::vector<Entity>& powerUps)
{
	for (auto &powerUp : powerUps)
	{
		switch (powerUp.m_dir.dirX)
		{
		case -1: powerUp.m_pos.x -= m_movementSpeed; break;
		case 1: powerUp.m_pos.x += m_movementSpeed; break;
		}

		switch (powerUp.m_dir.dirY)
		{
		case -1: powerUp.m_pos.y -= m_movementSpeed; break;
		case 1: powerUp.m_pos.y += m_movementSpeed; break;
		}
	}
}

