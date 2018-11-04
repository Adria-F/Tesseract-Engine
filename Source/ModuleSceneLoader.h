#ifndef __MODULESCENELOADER_H__
#define __MODULESCENELOADER_H__

#include "Module.h"
#include <vector>

class GameObject;
struct aiScene;
struct aiNode; 
struct aiMesh;

struct Mesh;
struct Texture;

class ModuleSceneLoader : public Module
{
public:
	ModuleSceneLoader(bool start_enabled = true);
	~ModuleSceneLoader();

	bool Init(JSON_File* document);

	bool CleanUp();

	bool importFBXScene(const char* path);
	GameObject* loadGO(const aiScene* scene, aiNode* node, std::vector<Mesh*> meshes, std::vector<Texture*> textures);

	bool saveScene(const char* scene_name);
	bool loadScene(const char* scene_name);

public:

	std::string next_loadedScene_name = "";

private:

	int root_counter;	
};

#endif // !__MODULESCENELOADER_H__