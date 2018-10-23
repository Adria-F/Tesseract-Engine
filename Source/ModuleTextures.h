#ifndef __MODULETEXTURES_H__
#define __MODULETEXTURES_H__

#include "Module.h"
#include <list>

struct Texture
{
	uint width = 0;
	uint height = 0;

	uint id = 0;
};

class ModuleTextures : public Module
{
public:
	ModuleTextures(bool start_enabled = true);
	~ModuleTextures();

	bool importTexture(const char* path);
	Texture* loadTexture(const char* path);

public:

	std::list<Texture*> textures;
};

#endif // !__MODULETEXTURES_H__