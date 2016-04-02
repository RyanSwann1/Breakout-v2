#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>

struct MoveDirection;
class Game;
class Player
{
public:
	Player(const Game& game);
	~Player();

	void movement(const MoveDirection& dir, const Game& game);
	void extendSizeWidth() { m_pos.w += 5; }

	SDL_Rect getPos() const { return m_pos; }

private:
	SDL_Rect m_pos; //Position of player

	int m_sizeWidth;
	int m_sizeHeight;
	int m_movementSpeed;
};
#endif // !PLAYER_H
