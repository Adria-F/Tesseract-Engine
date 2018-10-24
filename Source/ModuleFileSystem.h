#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#include "Module.h"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(bool start_enabled = true);
	~ModuleFileSystem();

	bool addPath(const char* path);
	bool fileExists(const char* path);

	uint readFile(const char* path, char** buffer);
	uint writeFile(const char* path, const void* buffer, uint size);

	//Manage saving files with same name adding (num_version) at the end of filename
	//returns num_version
	uint duplicateFile(const char* path, const void* buffer, uint size);

	void splitPath(const char* full_path, std::string* path, std::string* filename, std::string* extension);
	std::string normalizePath(const char* path);

	void manageDroppedFiles(const char* path);
};

#endif // !__MODULEFILESYSTEM_H__