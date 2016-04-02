#include "Game.h"
#include "Window.h"
#include "Player.h"
#include "Ball.h"
#include "MoveDirection.h"
#include "PowerUp.h"


#include <random>
#include <iostream>

Game::Game(const Window& window)
{
	m_gameOver = false;
	m_holdBall = true;
	m_playingFieldWidth = window.getScreenWidth();
	m_playingFieldHeight = window.getScreenHeight();
	m_spawnNewBallPosX = 0;
	m_spawnNewBallPosY = 0;
	spawnBlocks();
}


Game::~Game()
{
}

void Game::inputManager(Player & player, const Game& game, Ball& ball)
{
	MoveDirection dir;
	const Uint8 *keyState = SDL_GetKeyboardState(nullptr);

	//Move player to the right
	if (keyState[SDL_SCANCODE_D]) 
	{
		dir.dirX = 1;
		dir.dirY = 0;
		player.movement(dir, game);
	}

	//Move player left
	else if (keyState[SDL_SCANCODE_A]) 
	{
		dir.dirX = -1;
		dir.dirY = 0;
		player.movement(dir, game);
	}

	//Release ball
	else if (keyState[SDL_SCANCODE_SPACE]) 
	{
		if (m_holdBall) 
		{
			m_holdBall = false;
			ball.releaseBall(m_balls);
		}
	}

	if (!m_holdBall)
	{
		if (keyState[SDL_SCANCODE_P])
		{
			SDL_Rect pos;
			pos.x = 300;
			pos.y = 300;
			pos.h = 10;
			pos.w = 10;
			int movementSpeed = 6;
			spawnBall(pos, movementSpeed);
		}
	}

}

void Game::render(const Player & player, const Ball & ball, const Window& window)
{
	SDL_RenderClear(window.getRenderer());

	//Render player
	SDL_SetRenderDrawColor(window.getRenderer(), 255, 255, 255, 255);
	SDL_RenderFillRect(window.getRenderer(), &player.getPos());
	SDL_RenderFillRect(window.getRenderer(), &ball.getPos());
	//Render balls
	for (auto ballInstance : m_balls)
	{
		SDL_RenderFillRect(window.getRenderer(), &ballInstance.m_pos);
	}

	for (auto block : m_blocks) {
		SDL_RenderFillRect(window.getRenderer(), &block.m_pos);
	}
	//Render Extra Size
	for (auto extraSize : m_extendPlayerSize) {
		SDL_RenderFillRect(window.getRenderer(), &extraSize.m_pos);
	}
	
	//Render background
	SDL_SetRenderDrawColor(window.getRenderer(), 0, 0, 0, 0);

	//Apply render
	SDL_RenderPresent(window.getRenderer());
}

void Game::update(Game& game, Player& player, Ball& ball, PowerUp& powerUp)
{

	ball.movement(game, player, m_balls);
	ball.manageBounds(game, m_balls);

	powerUp.movement(game, m_extendPlayerSize);
	//Handle collisions
	collisionManagement(player, ball, game, powerUp);
}

void Game::spawnBall(SDL_Rect pos, const int movementSpeed)
{
	if (m_balls.empty()) {
		m_balls.push_back(Entity(pos, movementSpeed));
	}
	else
	{
		bool validPos = false;
		MoveDirection dir;
		while (!validPos) 
		{
			dir.dirX = getRandNumb(-1, 1);
			dir.dirY = getRandNumb(-1, 1);

			if (dir.dirX != 0 && dir.dirY != 0) {
				validPos = true;
			}
		}

		m_balls.push_back(Entity(pos, movementSpeed, dir));
	}
	
}




int Game::getRandNumb(const int min, const int max) const
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(min, max);

	return dis(gen);
}

void Game::collisionManagement(Player & player, Ball & ball, const Game& game, PowerUp& powerUp)
{
	//Ball to player collision
	for (auto &ballsInstance : m_balls)
	{
		if (ball.collision(ballsInstance.m_pos, player.getPos()))
		{
			std::cout << "Collision";
			//Send in center of ball

			ballsInstance.m_dir = ball.getReflection(player.getPos(), ballsInstance.m_pos.x + (ballsInstance.m_pos.w / 2), ballsInstance.m_dir);
			//ball.changeDir(ball.getReflection(player.getPos(), ball.getPos().x + (ball.getPos().w / 2)));
		}
	}

	//Ball to block collision
	for (auto &ballInstance : m_balls)
	{
		for (auto cIter = m_blocks.cbegin(); cIter != m_blocks.cend(); cIter++)
		{
			//If collision
			if (ball.collision(ballInstance.m_pos, cIter->m_pos))
			{
				m_spawnNewBallPosX = cIter->m_pos.x;
				m_spawnNewBallPosY = cIter->m_pos.y;

				ballInstance.m_dir = ball.getBlockReflection(cIter->m_pos, ballInstance.m_pos.x + (ballInstance.m_pos.w / 2), ballInstance.m_dir);
				m_blocks.erase(cIter);
				//Spawn extra ball
				if (getRandNumb(1, 8) == 4)
				{

					SDL_Rect pos;
					pos.x = m_spawnNewBallPosX;
					pos.y = m_spawnNewBallPosY;
					pos.w = ball.getWidth();
					pos.h = ball.getHeight();
					int movementSpeed = ball.getMovementSpeed();
					spawnBall(pos, movementSpeed);

					//spawnExtraBall(ball, game);
				}
				if (getRandNumb(1, 6) == 3) {
					spawnExtraPlayerSize(player, ball);
				}
				return;
			}
		}
	}

	//Power up/Ball collision
	for (auto cIter = m_extendPlayerSize.cbegin(); cIter != m_extendPlayerSize.cend(); cIter++)
	{
		if (powerUp.collision(cIter->m_pos, player.getPos()))
		{
			player.extendSizeWidth();
			m_extendPlayerSize.erase(cIter);
			break;
		}
	}


	////if Ball and Block collision
	//for (auto cIter = m_blocks.cbegin(); cIter != m_blocks.cend(); cIter++)
	//{
	//	if (ball.collision(cIter->m_pos)) 
	//	{
	//		ball.changeDir(ball.getBlockReflection(cIter->m_pos, ball.getPos().x + (ball.getPos().w / 2)));
	//		m_blocks.erase(cIter);
	//		break;
	//	}
	//}

}

void Game::spawnExtraPlayerSize(Player & player, const Ball& ball)
{
	SDL_Rect pos;
	pos.x = m_spawnNewBallPosX;
	pos.y = m_spawnNewBallPosY;
	pos.w = ball.getWidth();
	pos.h = ball.getHeight();
	//Move DIrection of power up
	MoveDirection dir;
	dir.dirX = 0;
	dir.dirY = 1;
	int movementSpeed = ball.getMovementSpeed() - (ball.getMovementSpeed() / 2);
	m_extendPlayerSize.push_back(Entity(pos, movementSpeed, dir));
}


void Game::spawnExtraBall(const Ball & ball, const Game& game)
{
	//Position of ball
	SDL_Rect pos;
	pos.x = m_spawnNewBallPosX;
	pos.y = m_spawnNewBallPosY;
	pos.w = ball.getWidth();
	pos.h = ball.getHeight();
	//Move direction of ball
	MoveDirection dir;
	dir.dirX = ball.getRandBallDir(game);
	dir.dirY = ball.getRandBallDir(game);
	//Movement speed of ball
	int movementSpeed = ball.getMovementSpeed();
	m_balls.push_back(Entity(pos, movementSpeed, dir));
}

void Game::spawnBlocks()
{
	static constexpr int SIZE_WIDTH = 50;
	static constexpr int SIZE_HEIGHT = 10;
	int spawnXPos = 100;
	int spawnYPos = 100;
	int newXPos = 60;
	int newYPos = 50;

	for (int i = 0; i != TOTAL_BLOCKS; i++)
	{
		m_blocks.push_back(Entity({ spawnXPos, spawnYPos, SIZE_WIDTH, SIZE_HEIGHT }));
		spawnXPos += newXPos;

		if (i == 8 || i == 17)
		{
			spawnYPos += newYPos;
			spawnXPos = 100;
		}
	}
}

