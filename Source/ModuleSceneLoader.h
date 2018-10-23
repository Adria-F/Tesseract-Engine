#ifndef __MODULESCENELOADER_H__
#define __MODULESCENELOADER_H__

#include "Module.h"

class ModuleSceneLoader : public Module
{
public:
	ModuleSceneLoader();
	~ModuleSceneLoader();

	//bool loadSceneFromFile();
	bool importFBXScene();
};

#endif // !__MODULESCENELOADER_H__