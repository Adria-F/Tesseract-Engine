#ifndef __MODULESCENELOADER_H__
#define __MODULESCENELOADER_H__

#include "Module.h"
#include <vector>

class GameObject;
class ResourceMesh;
class ResourceTexture;

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

	bool importFBXScene(const char* path, uint UID, std::vector<uint>& meshesUIDs, std::vector<uint>& animationsUIDs, std::string& newPath,JSON_Value* meta, bool newMeta = false);
	GameObject* loadGameObject(const aiScene* scene, aiNode* node, std::vector<ResourceMesh*> meshes, std::vector<ResourceTexture*> textures, GameObject* fakeScene);

	bool saveSceneFile(const char* scene_name, GameObject* fakeRoot = nullptr);
	bool saveVirtualScene();

	bool loadSceneFile(const char* scene_name, bool isFBX = false);
	bool loadVirtualScene();

	bool saveScene(JSON_File* scene, GameObject* rootObject);
	bool loadScene(JSON_File* scene);

public:

	std::string next_scene_path = "";

private:

	int root_counter=0;	
};

#endif // !__MODULESCENELOADER_H__