#ifndef RENDERER3D_H
#define RENDERER3D_H

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Mesh.h"

#define EXPORTDLL _declspec(dllexport)

class EXPORTDLL Renderer3D
{
public:
	Renderer3D(class Window* window);
	~Renderer3D();
	void Draw(class Entity3D* root);
	void RenderEntity(Entity3D* toRender);
	void SetTextures(Mesh* toRender, vector<TextureStruct> textures);
	void SetBackgroundColor(float r, float g, float b, float a) const;
	class Camera* GetCamera();
	glm::mat4 GetProjMatrix();

private:
	unsigned int uniView;
	glm::mat4 projMatrix;
	GLFWwindow* renderWindow;
	Camera* renderCamera;
};

#endif