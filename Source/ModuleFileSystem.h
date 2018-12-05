#ifndef __MODULEFILESYSTEM_H__
#define __MODULEFILESYSTEM_H__

#include "Module.h"
#include "Panel.h"
#include "Timer.h"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(bool start_enabled = true);
	~ModuleFileSystem();

	bool Start();

	update_status Update(float dt);

	bool addPath(const char* path);
	bool removePath(const char* path);
	void addPathOfFilesAt(const char* path);

	bool fileExists(const char* path, const char* atDirectory = nullptr, const char* withExtension = nullptr);

	uint readFile(const char* path, char** buffer);
	uint writeFile(const char* path, const void* buffer, uint size, bool overwrite = false);

	bool copyFile(const char* src, const char* dest, bool deleteSource = false);
	bool createDirectory(const char* path);
	bool deleteFile(const char* path);
	bool renameFile(const char* path, const char* name);
	
	//Check if the file exists and returns num_version
	//num_version is equivalent to the amounts of times that a file with given name exists -1
	//Modify string to: path(num_version)
	uint getAvailablePath(const char* originalPath, std::string* path);
	uint getAvailableNameFromArray(std::vector<std::string*> names, std::string& name);

	void splitPath(const char* full_path, std::string* path, std::string* filename, std::string* extension);
	std::string normalizePath(const char* path);
	std::string getFullPath(const char* path, const char* atDirectory = nullptr, const char* withExtension = nullptr);
	std::string getRealDirectory(const char* path);

	//Returns the UID of the resource if some has been created, 0 if not
	uint manageDroppedFiles(const char* path);

	void importFilesAt(const char* path, bool firstTime = false);
	//This will delete all .meta and library files to make a clean re-import of all assets
	void reimportFiles();
	void deleteMetasAt(const char* path);
	
	void getFilesAt(const char* path, std::list<assetsElement*>& elements, const char* exclusiveExtension = nullptr, const char* ignoreExtension = nullptr);

	int getLastTimeChanged(const char* path) const;
	//Path: path of the asset file (not the .meta)
	int getMetaLastChange(const char* path) const;
	//Returns true if the file has been modified, according to .meta or if there is no .meta
	bool isFileModified(const char* path) const;

private:

	Timer import_timer;
	float import_delay = 1000.0f;
};

#endif // !__MODULEFILESYSTEM_H__