#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"
#include "JSONManager.h"
#include <string>

enum ResType {

	NO_TYPE,
	R_MESH,
	R_TEXTURE,
	R_COLOR,
	R_SCENE,
	R_ANIMATION,
	R_BONE
};

class Resource
{
	friend class ModuleResource;
	friend class ModuleSceneLoader;
	friend class ModuleMeshes;

public:

	Resource(uint UID, ResType type);
	virtual ~Resource() {};

	ResType GetType()const;
	uint GetUID()const;
	const char* GetName() const;
	const char* GetFile()const;
	const char* GetExportedFile()const;
	bool IsLoaded()const;
	//You should call this function
	bool LoadtoMemory();
	//You should call this function
	bool UnloadMemory();
	uint GetTimesLoaded() const;

	virtual bool LoadInMemory() { return false; }
	virtual bool UnloadFromMemory() { return false; }

protected:

	std::string name;

	uint UID = 0;
	uint loaded = 0;

	std::string file="";
	std::string exported_file="";

	ResType type = NO_TYPE;
	
};

#endif //__RESOURCE_H__