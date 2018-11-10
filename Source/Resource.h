#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"
#include <string>

enum ResType {

	NO_TYPE,
	MESH,
	TEXTURE,
	SCENE
};

class Resource
{
	friend class ModuleResource;

public:

	Resource(uint UID, ResType type);
	virtual ~Resource() {};

	ResType GetType()const;
	uint GetUID()const;
	const char* GetFile()const;
	const char* GetExportedFile()const;
	bool IsLoaded()const;
	bool Load()const;
	uint GetTimesLoaded() const;

	virtual void Save()const {};
	virtual void Load() {};
	virtual bool LoadInMemory() { return false; }

protected:

	uint UID = 0;
	uint loaded = 0;

	std::string file;
	std::string exported_file;

	ResType type = NO_TYPE;
	
};

#endif //__RESOURCE_H__