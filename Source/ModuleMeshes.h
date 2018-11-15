#ifndef __MODULEMESHES_H__
#define __MODULEMESHES_H__

#include "Module.h"

#include <string>

struct Mesh;
struct aiMesh;
class ResourceMesh;
class Resource;

class ModuleMeshes : public Module
{
public:
	ModuleMeshes(bool start_enabled = true);
	~ModuleMeshes() {}

	bool importRMesh(aiMesh* mesh, uint UID, std::string& path);
	Mesh* loadMesh(const char* meshName);
	bool saveMesh(ResourceMesh* mesh, uint UID, std::string& newpath);
	
};

#endif //__MODULEMESHES_H__