#ifndef __MODULEMESHLOADER_H__
#define __MODULEMESHLOADER_H__

#include "Module.h"

struct Mesh;

class ModuleMeshLoader : public Module
{
public:
	ModuleMeshLoader(bool start_enabled = true);
	~ModuleMeshLoader();

	void loadMesh(Mesh& mesh);
};

#endif //__MODULEMESHLOADER_H__