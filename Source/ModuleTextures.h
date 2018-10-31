#ifndef __MODULETEXTURES_H__
#define __MODULETEXTURES_H__

#include "Module.h"
#include <list>

struct Texture
{
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

	bool importTexture(const char* path);
	Texture* loadTexture(const char* path);

	Texture* loadIcon(const char* path);

public:

	std::list<Texture*> textures;
	std::list<Texture*> icons;
};

#endif // !__MODULETEXTURES_H__