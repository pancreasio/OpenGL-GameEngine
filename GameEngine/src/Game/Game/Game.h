#ifndef GAME_H
#define GAME_H

#include "Engine/BaseGame.h"
#include "Engine/Shape.h"
#include "Engine/Sprite.h"

class Game : public BaseGame
{
//private:
//	BaseGame* bg;
public:
	Game(int width, int height);
	~Game();
	void Init();
	void Update(const float deltaTime);
private:
	Shape* shape;
	Shape* shape2;
	Sprite* anim;
};

#endif