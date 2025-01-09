#ifndef __TEST_GAME_H__
#define __TEST_GAME_H__

#include "../engine/AbstractGame.h"
#include <vector>

struct GameKey {
	Point2 pos;
	bool isAlive;
};

struct Ball {
	Point2 pos;
	Vector2i velocity;
	int radius;
	bool isLaunched;
};

struct Peg {
	Point2 pos;
	int radius; 
	bool isHit; 
};

class MyGame : public AbstractGame {
private:
	Rect box;

	Vector2i velocity;

	std::vector<std::shared_ptr<GameKey>> gameKeys;
	std::vector<std::shared_ptr<Peg>> pegs;

	Ball ball; // Add Ball member

	/* GAMEPLAY */
	int score, numKeys, lives;
	bool gameWon;
	bool gameOver;
	int endMessageTimer;
	const int endMessageDuration = 120;

	void handleKeyEvents();
	void update();
	void render();
	void renderUI();
	void resetGame();
	void limitBalls();
	int ballsLaunched;
public:
	MyGame();
	~MyGame();
};

#endif
