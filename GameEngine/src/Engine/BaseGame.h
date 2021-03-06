#ifndef BASEGAME_H
#define BASEGAME_H
#define EXPORTDLL _declspec(dllexport)

#include <list>

#include "Window.h"
#include "Renderer.h"
#include "Input.h"
#include "Cursor.h"
#include "CollisionManager.h"

class Entity;

class EXPORTDLL BaseGame
{
public:
	BaseGame(int screenWidth, int screenHeight);
	~BaseGame();
	int GameLoop();
	virtual void Update(const float deltaTime) = 0;
	std::list<Entity*> static entityList;
private:
	//Renderer* renderer;
	class Renderer3D* renderer3d;
	float deltaTime;
	float lastFrameTime;
protected:
	Window* window;
	Input* input;
	Cursor* cursor;
	CollisionManager* collisionManager;
	Camera* renderCamera;
	class Entity3D* sceneRoot;
};

#endif