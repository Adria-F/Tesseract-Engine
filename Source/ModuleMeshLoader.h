#ifndef __MODULEMESHLOADER_H__
#define __MODULEMESHLOADER_H__

#include "Module.h"

struct Mesh;
struct aiScene;
struct aiNode;

class GameObject;

class ModuleMeshLoader : public Module
{
public:
	ModuleMeshLoader(bool start_enabled = true);
	~ModuleMeshLoader() {}

	bool Init(rapidjson::Document& document);

	bool Start();


	update_status PreUpdate(float dt);


	update_status Update(float dt);


	update_status PostUpdate(float dt);

	bool CleanUp();

	void ImportFBX(const char* full_path);
	void LoadGameObjects(const aiScene* scene,aiNode* node, GameObject* parent);
	void loadNodeMesh(const aiScene* scene, aiNode* node, std::string meshPath);
	GLuint loadTexture(const char* path, uint& width, uint& height);
	
public:

	//Saved used texture info
	std::string usedPath = "";
	uint usedTexture = 0;
	uint usedTextureWidth = 0;
	uint usedTextureHeight = 0;
};

#endif //__MODULEMESHLOADER_H__