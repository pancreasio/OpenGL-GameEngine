#include "Entity.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include"glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"

#include "LoadShader.h"
#include "Texture.h"
#include "Renderer.h"
#include "BaseGame.h"

Entity::Entity()
{
	position = { 0, 0, 0 };
	BaseGame::entityList.push_front(this);
}

Renderer* Entity::renderer = nullptr;

Entity::Entity(glm::vec3 pos, Texture* tex)
{
	position = pos;
	BaseGame::entityList.push_front(this);
	texture = tex;
}

void Entity::SetShader()
{
	//load shaders
	programID = LoadShaders("../src/Engine/SimpleVertexShader.vertexshader", "../src/Engine/SimpleFragmentShader.fragmentshader");

	//use shader
	glUseProgram(programID);

	// Specify the layout of the vertex data
	posAttrib = glGetAttribLocation(programID, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(posAttrib);

	colAttrib = glGetAttribLocation(programID, "customColor");
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(colAttrib);

	texAttrib = glGetAttribLocation(programID, "aTexCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	model = glm::mat4(1.0f);
	uniModel = glGetUniformLocation(programID, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	glm::mat4 view = renderer->GetViewMatrix();
	GLint uniView = glGetUniformLocation(programID, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = renderer->GetProjMatrix();
	GLint uniProj = glGetUniformLocation(programID, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	glUniform1i(glGetUniformLocation(programID, "tex"), 0);

	//glActiveTexture(texture->GetTextureIndex());
	//glBindTexture(GL_TEXTURE_2D, texture->GetTexture());
}

void Entity::Rotate(float angle, glm::vec3 axis)
{
	model = glm::rotate(model, glm::radians(angle), axis);
}

void Entity::Scale(glm::vec3 scaleValues)
{
	model = glm::scale(model, scaleValues);
	scale *= scaleValues;
	std::cout << scale.x << std::endl;
}

void Entity::Translate(float value, glm::vec3 axis)
{
	model = glm::translate(model, value * (axis* 0.01f));
	position += value * axis;
	position = glm::project(value*axis, model, renderer->GetProjMatrix(), glm::vec4(0, 0, 800, 600));
}

void Entity::Render() 
{

}

glm::vec2 Entity::GetPosition() 
{
	return position;
}

glm::vec2 Entity::GetScale() 
{
	return scale;
}