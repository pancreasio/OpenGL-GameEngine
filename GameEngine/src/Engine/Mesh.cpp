#include "Mesh.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

//#include "Entity.h"
#include "Shader.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<TextureStruct> textures, Entity3D* newParent, Shader* shader): Entity3D(glm::vec3(1.f),newParent, shader)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	//this->parent = newParent;

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -100.75f, 0.0f));
	model = glm::scale(model, glm::vec3(10.8f, 10.8f, 10.8f));
	
	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	SetupMesh();
}

int Mesh::GetElementsSize()
{
	return indices.size();
}

unsigned Mesh::GetVertexArray()
{
	return VAO;
}

vector<TextureStruct> Mesh::GetTextures()
{
	return textures;
}

void Mesh::Draw()
{
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		string name = textures[i].type;
		
		//string number;
		//if (name == "texture_diffuse")
		//	number = std::to_string(diffuseNr++);
		//else if (name == "texture_specular")
		//	number = std::to_string(specularNr++); // transfer unsigned int to stream
		//else if (name == "texture_normal")
		//	number = std::to_string(normalNr++); // transfer unsigned int to stream
		//else if (name == "texture_height")
		//	number = std::to_string(heightNr++); // transfer unsigned int to stream
	
		// now set the sampler to the correct texture unit
		shader->SetInt(name.c_str(), i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
	
	Entity3D::Draw();
}

void Mesh::SetupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	glBindVertexArray(0);
}