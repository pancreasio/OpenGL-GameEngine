#include "Game.h"
#include "Engine/BaseGame.h"
#include "Engine/Texture.h"
#include "Engine/Shader.h"
#include "Engine/DirectionalLight.h"

#include <iostream>

#include "FirstPersonCameraController.h"

Game::Game(int width, int height) : BaseGame(width, height)
{

}

Game::~Game()
{
	delete shape;
	delete shape2;
	delete anim;
	delete cursor;
	delete ourShader;
}

void Game::Init()
{
	Texture bokeTex("../res/BOKEE.png");
	Texture comuTex("../res/texture.png");
	Texture animTex("../res/anim.png");

	/*shape = new Shape({ 0,0,0 }, { 100,100,0 }, &bokeTex);
	shape2 = new Shape({ 30.f,30.f,0 }, { 100,100,0 }, &comuTex);
	anim = new Sprite({ 0,0,0 }, { 368, 200, 0 }, &animTex);
	anim->CreateAnimation(368, 368 / 8, 4);
	anim->SetCurrentAnimation(8, 0, 2);*/
	ourShader = new Shader("../src/Engine/3DVertexShader.vertexshader", "../src/Engine/3DFragmentShader.fragmentshader");
	safePositionExists = false;
	safePosition = { 0.f,0.f };
	fpsCamera = new FirstPersonCameraController(renderCamera, cursor);
	fpsCamera->SetSensitivity(0.3f);
	//shader initialization
	ourShader->SetVec3("viewPosition", renderCamera->GetPosition());
	ourShader->SetVec3("viewDirection", renderCamera->GetDirection());

	dirLight = new DirectionalLight(glm::vec3 { 0.f,0.5f,-1.f }, glm::vec3{ 0.1f,0.1f,0.1f },
		glm::vec3{ 0.8f,0.5f,0.5f }, glm::vec3{ 1.f,1.f,1.f });
	
	/*glm::vec3 dir = { 0.f,0.f,1.f };
	glm::vec3 ambient = { 1.f,1.f,1.f };
	glm::vec3 diffuse = { 1.f,1.f,1.f };
	glm::vec3 specular = { 1.f,1.f,1.f };*/

	
	/*ourShader->SetVec3("lightDirection", dir);
	ourShader->SetVec3("lightAmbient", ambient);
	ourShader->SetVec3("lightDiffuse", diffuse);
	ourShader->SetVec3("lightSpecular", specular);*/
	
	testModel = new Model("../res/model/backpack.obj", ourShader);
	GameLoop();
}

void Game::Update(const float deltaTime)
{
	/*glm::vec3 dir = { 0.f,0.5f,-1.f };
	glm::vec3 ambient = { 0.1f,0.1f,0.1f };
	glm::vec3 diffuse = { 0.8f,0.5f,0.5f };
	glm::vec3 specular = { 1.f,1.f,1.f };

	ourShader->SetVec3("lightDirection", dir);
	ourShader->SetVec3("lightAmbient", ambient);
	ourShader->SetVec3("lightDiffuse", diffuse);
	ourShader->SetVec3("lightSpecular", specular);*/

	ourShader->SetVec3("lightDirection", dirLight->dir);
	ourShader->SetVec3("lightAmbient", dirLight->ambient);
	ourShader->SetVec3("lightDiffuse", dirLight->diffuse);
	ourShader->SetVec3("lightSpecular", dirLight->specular);
	
	//scaling
	//anim->UpdateCurrentAnimation(deltaTime);

	//if (input->GetKey(GLFW_KEY_H))
	//{
	//	anim->Scale({ 1.1f,1.1f,1.1f });
	//}

	//if (input->GetKey(GLFW_KEY_K))
	//{
	//	anim->Scale({ 0.9f,0.9f,0.9f });
	//}

	//if (input->GetKey(GLFW_KEY_N))
	//{
	//	shape2->Scale({ 1.1f,1.1f,1.1f });
	//}

	//if (input->GetKey(GLFW_KEY_M))
	//{
	//	shape2->Scale({ 0.9f,0.9f,0.9f });
	//}

	////rotating
	//if (input->GetKey(GLFW_KEY_E))
	//{
	//	anim->Rotate(-1.0f, { 0.0f,0.0f,1.f });
	//	anim->SetCurrentAnimation(8, 2, 2);
	//}

	//if (input->GetKey(GLFW_KEY_F))
	//{
	//	anim->Rotate(-1.0f, { 0.0f,1.0f,0.f });
	//	anim->SetCurrentAnimation(8, 2, 2);
	//}

	//else if (input->GetKey(GLFW_KEY_Q))
	//{
	//	anim->Rotate(1.0f, { 0.0f,0.0f,1.f });
	//	anim->SetCurrentAnimation(6, 1, 2);
	//}

	//translating

	//if (input->GetKey(GLFW_KEY_LEFT))
	//{
	//	anim->Translate(-60.f, { 1.0f,0.0f,0.0f });
	//	anim->SetCurrentAnimation(8, 0, 2);
	//}

	//if (input->GetKey(GLFW_KEY_RIGHT))
	//{
	//	anim->Translate(60.f, { 1.0f,0.0f,0.0f });
	//	anim->SetCurrentAnimation(8, 0, 2);
	//}

	//if (input->GetKey(GLFW_KEY_UP))
	//{
	//	anim->Translate(60.f, { 0.0f,1.0f,0.0f });
	//	anim->SetCurrentAnimation(8, 0, 2);
	//}
	//if (input->GetKey(GLFW_KEY_DOWN))
	//{
	//	anim->Translate(-60.f, { 0.0f,1.0f,0.0f });
	//	anim->SetCurrentAnimation(8, 0, 2);
	//}

	////collisions

	//if (collisionManager->CheckCollision(*shape, *shape2))
	//{
	//	if (safePositionExists)
	//		shape2->SetPosition(safePosition);
	//}
	//else
	//{
	//	safePosition = shape2->GetPosition();
	//	safePositionExists = true;
	//}

	//if (input->GetKey(GLFW_KEY_SPACE))
	//{
	//	shape2->SetPosition({ 30.f,30.f });
	//}

	if (cursor->GetCursorMode() != Cursor::CursorMode::capture)
		cursor->SetCursorMode(Cursor::CursorMode::capture);

	if (input->GetKey(GLFW_KEY_X))
	{
		renderCamera->RotatePitch(10.f);
	}
	
	testModel->Draw();

	//camera

	if (input->GetKey(GLFW_KEY_A))
	{
		fpsCamera->TranslateX(-10.f);
	}

	if (input->GetKey(GLFW_KEY_D))
	{
		fpsCamera->TranslateX(10.f);
	}

	if (input->GetKey(GLFW_KEY_W))
	{
		fpsCamera->TranslateY(10.f);
	}
	if (input->GetKey(GLFW_KEY_S))
	{
		fpsCamera->TranslateY(-10.f);
	}

	fpsCamera->Update();
}