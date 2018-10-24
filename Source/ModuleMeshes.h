#ifndef __MODULEMESHES_H__
#define __MODULEMESHES_H__

#include "Module.h"

struct Mesh;
struct aiMesh;

class ModuleMeshes : public Module
{
public:
	ModuleMeshes(bool start_enabled = true);
	~ModuleMeshes() {}

	Mesh* importMesh(aiMesh mesh);
	Mesh* loadMesh(const char* path);
	bool saveMesh(Mesh* mesh);
	
};

#endif //__MODULEMESHES_H__