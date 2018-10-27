#ifndef __MODULESCENELOADER_H__
#define __MODULESCENELOADER_H__

#include "Module.h"

class GameObject;
struct aiScene;
struct aiNode;

class ModuleSceneLoader : public Module
{
public:
	ModuleSceneLoader(bool start_enabled = true);
	~ModuleSceneLoader();

	bool Init(rapidjson::Document& document);

	bool CleanUp();

	//bool loadSceneFromFile();
	bool importFBXScene(const char* path);
	bool importFSScene(const char* path);

	void LoadGameObjects(const aiScene* scene, aiNode* node, GameObject* parent);

private:
	int root_counter;
};

#endif // !__MODULESCENELOADER_H__