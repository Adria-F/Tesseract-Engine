#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#include "Module.h"
#include "PanelAssets.h"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(bool start_enabled = true);
	~ModuleFileSystem();

	bool Start();

	bool addPath(const char* path);
	bool fileExists(const char* path, const char* atDirectory = nullptr, const char* withExtension = nullptr);

	uint readFile(const char* path, char** buffer);
	uint writeFile(const char* path, const void* buffer, uint size);

	//Manage saving files with same name adding (num_version) at the end of filename
	//returns num_version
	uint duplicateFile(const char* path, const void* buffer, uint size);

	void splitPath(const char* full_path, std::string* path, std::string* filename, std::string* extension);
	std::string normalizePath(const char* path);

	void manageDroppedFiles(const char* path);

	void importAssets();
	void importFilesAt(const char* path);
	
	void getAssetsFiles();
	void getFilesAt(const char* path, assetsElement* element = nullptr);
};

#endif // !__MODULEFILESYSTEM_H__