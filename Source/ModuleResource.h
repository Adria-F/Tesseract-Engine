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

	JSON_File* getMeta(const char* path) const;
	JSON_File* createMeta(const char* path, ResType type) const;

private:

	uint last_UID = 1;
	std::map <uint, Resource* > resources;

};

#endif // !__MODULERESOURCE_H__
