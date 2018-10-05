#ifndef __MODULEMESHLOADER_H__
#define __MODULEMESHLOADER_H__

#include "Module.h"

struct Mesh;

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
	GLuint loadTexture(const char* path);
	
};

#endif //__MODULEMESHLOADER_H__