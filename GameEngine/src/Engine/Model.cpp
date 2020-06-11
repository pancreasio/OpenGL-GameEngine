#include "Model.h"

#include "Mesh.h"
#include "Texture.h"
//#include "LoadShader.h"
#include "Shader.h"

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>      
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Entity.h"
#include "Entity3D.h"

Model::Model(string const &path, Shader* shader)
{
	LoadModel(path);
	this->shader = shader;

	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(0.0f, -100.75f, 0.0f));
	modelMat = glm::scale(modelMat, glm::vec3(10.8f, 10.8f, 10.8f));
}

void Model::LoadModel(string const &path)
{	
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene, nullptr);
}

void Model::Draw()
{
	shader->Use();
	shader->SetMat4("model", modelMat);
	
	glm::mat4 proj = Entity::renderer->GetProjMatrix();
	glm::mat4 view = Entity::renderer->GetCamera()->GetViewMatrix();

	//unsigned int uniView = glGetUniformLocation(programID, "view");
	//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
	//unsigned int uniProj = glGetUniformLocation(programID, "proj");
	//glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	shader->SetMat4("view", view);
	shader->SetMat4("proj", proj);
	
	root->Draw(shader);
}

void Model::ProcessNode(aiNode *node, const aiScene *scene, Entity3D* parent)
{
	Entity3D* thisNode = nullptr;
	if(node->mNumMeshes == 0)
	{
		thisNode = new Entity3D(glm::vec3(0.f), parent, shader);
		if (parent == nullptr)
			root = thisNode;
		nodes.push_back(thisNode);
	}
	else
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			thisNode = new Mesh(ProcessMesh(mesh, scene, parent, shader));
			nodes.push_back(static_cast<Mesh*>(thisNode));
		}
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, thisNode);
	}
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene, Entity3D* parent, Shader* shader)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<TextureStruct> textures;

	if (mesh->mNumVertices <= 1)
		std::cout << "zarlanga";
	
	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;
		// normals
		if (mesh->mNormals != NULL)
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}
		else
			vertex.normal = glm::vec3(0.0f);
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		if (mesh->mTangents != NULL)
		{
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;
		}
		else
			vertex.tangent = glm::vec3(0.0f);
		if (mesh->mBitangents != NULL)
		{
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;
		}
		else
			vertex.bitangent = glm::vec3(0.0f);
		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	vector<TextureStruct> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<TextureStruct> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<TextureStruct> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<TextureStruct> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures, parent, shader);
}

void Model::Rotate(float angle, glm::vec3 axis)
{
	modelMat = glm::rotate(modelMat, glm::radians(angle), axis);
}

vector<TextureStruct> Model::LoadMaterialTextures(aiMaterial *mat, int type, string typeName)
{
	vector<TextureStruct> textures;
	aiTextureType texType = static_cast<aiTextureType>(type);
	for (unsigned int i = 0; i < mat->GetTextureCount(texType); i++)
	{
		aiString str;
		mat->GetTexture(texType, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			TextureStruct texture;
			string filename = string(str.C_Str());
			filename = directory + '/' + filename;
			Texture texId(filename.c_str());
			//texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.id = texId.GetTexture();
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}
