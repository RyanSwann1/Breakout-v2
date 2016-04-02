#ifndef POWERUP_H
#define POWERUP_H

#include <vector>
#include <SDL.h>
struct Entity;
class Game;
class PowerUp
{
public:
	PowerUp();
	~PowerUp();

	bool collision(const SDL_Rect& powerUpPos, const SDL_Rect& entityPos);
	void movement(const Game& game, std::vector<Entity>& powerUps);

private:
	int m_movementSpeed;
};

#endif // !POWERUP_H
