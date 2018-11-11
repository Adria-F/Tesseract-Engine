#ifndef __MODULEMESHES_H__
#define __MODULEMESHES_H__

#include "Module.h"

struct Mesh;
struct aiMesh;
class ResourceMesh;
class Resource;

class ModuleMeshes : public Module
{
public:
	ModuleMeshes(bool start_enabled = true);
	~ModuleMeshes() {}

	ResourceMesh* importRMesh(aiMesh* mesh);
	Mesh* loadMesh(const char* meshName);
	bool saveMesh(ResourceMesh* mesh, std::string& newpath);
	
};

#endif //__MODULEMESHES_H__