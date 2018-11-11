#ifndef __MODULETEXTURES_H__
#define __MODULETEXTURES_H__

#include "Module.h"
#include <list>
#include <string>

class ResourceTexture;

struct Texture
{
	~Texture();

	std::string name = "";

	uint width = 0;
	uint height = 0;

	uint GL_id = 0;
};

class ModuleTextures : public Module
{
public:
	ModuleTextures(bool start_enabled = true);
	~ModuleTextures();

	bool Init(JSON_File* document);

	bool CleanUp();

	bool importTexture(const char* path, std::string& newpath);
	bool saveTexture(const char* path, int type);
	Texture* loadTexture(const char* path);
	ResourceTexture* LoadResourceTexture(const char* path);

	Texture* loadIcon(const char* path);

public:

	std::list<Texture*> textures;
	std::list<ResourceTexture*> resourcetextures;
	std::list<Texture*> icons;
};

#endif // !__MODULETEXTURES_H__