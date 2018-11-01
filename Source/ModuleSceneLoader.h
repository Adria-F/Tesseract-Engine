#ifndef __MODULESCENELOADER_H__
#define __MODULESCENELOADER_H__

#include "Module.h"

class GameObject;
struct aiScene;
struct aiNode; 
struct aiMesh;

struct Mesh;

class ModuleSceneLoader : public Module
{
public:
	ModuleSceneLoader(bool start_enabled = true);
	~ModuleSceneLoader();

	bool Init(JSON_File* document);

	bool CleanUp();

	//bool loadSceneFromFile();
	bool importFBXScene(const char* path);
	bool importFSScene(const char* path);

	Mesh* LoadGOMesh(aiMesh* currentMesh);
	void LoadGameObjects(const aiScene* scene, aiNode* node, GameObject* parent);

	bool saveScene(const char* scene_name);
	bool loadScene(const char* scene_name);

private:
	int root_counter;
};

#endif // !__MODULESCENELOADER_H__