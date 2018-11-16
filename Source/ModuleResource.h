#ifndef __MODULERESOURCE_H__
#define __MODULERESOURCE_H__

#include "Module.h"
#include <map>

class Resource;
enum ResType;

class ModuleResource:public Module
{
public:
	ModuleResource(bool start_enabled = true);
	~ModuleResource();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	uint Find(const char* file) const;
	uint ImportFile(const char* file, ResType type);
	uint GenerateNewUID();
	Resource* GetResource(uint uid);
	Resource* AddResource(ResType type, uint forced_uid = 0);
	bool deleteResource(uint uid);
	uint GetResourceByFile(const char* file);

	JSON_File* getMeta(const char* path) const;
	JSON_File* createMeta(const char* path, ResType type) const;
	bool updateMetaLastChange(const char* path);
	bool updateMeshesUIDs(const char* path, std::map<std::string*, uint> meshesUIDs, JSON_Value* existingMeta = nullptr);
	//To get the UID of a mesh, enter the .fbx path + the mesh name
	uint getResourceUIDFromMeta(const char* path, const char* meshName = nullptr);

public:

	uint last_UID = 1;
	std::map <uint, Resource* > resources;

};

#endif // !__MODULERESOURCE_H__
