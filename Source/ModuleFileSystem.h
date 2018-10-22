#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#include "Module.h"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(bool start_enabled = true);
	~ModuleFileSystem();

	bool addPath(const char* path);

	void readFile(const char* path, char** buffer);
	void readFile(const char* path, float** buffer);
	void writeFile(const char* path, const void* buffer, uint size);
};

#endif // !__MODULEFILESYSTEM_H__