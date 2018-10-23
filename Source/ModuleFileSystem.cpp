#include "ModuleFileSystem.h"
#include "Application.h"
#include "PhysFS\include\physfs.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

ModuleFileSystem::ModuleFileSystem(bool start_enabled) : Module(start_enabled)
{
	//Init PHYSFS before Init to allow other modules to use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	if (PHYSFS_setWriteDir(".") == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	addPath(".");

	//Create main files if they do not exist and add them to the search path
	const char* mainPaths[] = {
		ASSETS_FOLDER, LIBRARY_FOLDER
	};
	for (uint i = 0; i < PATHS_AMOUNT; ++i)
	{
		if (PHYSFS_exists(mainPaths[i]) == 0)
			PHYSFS_mkdir(mainPaths[i]);

		addPath(mainPaths[i]);
	}
}

ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}

bool ModuleFileSystem::addPath(const char * path)
{
	bool ret = true;

	if (PHYSFS_mount(path, nullptr, 1) == 0)
	{
		LOG("PHYSFS - Error while adding path: %s", PHYSFS_getLastError());
		ret = false;
	}

	return ret;
}

void ModuleFileSystem::readFile(const char * path, char** buffer)
{
	PHYSFS_file* file = PHYSFS_openRead(path);
	if (file != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(file);
		*buffer = new char[size];
		PHYSFS_read(file, *buffer, 1, size);
		PHYSFS_close(file);
	}
}

void ModuleFileSystem::readFile(const char * path, float ** buffer)
{
	PHYSFS_file* file = PHYSFS_openRead(path);
	if (file != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(file);
		*buffer = new float[size];
		PHYSFS_read(file, *buffer, 1, size);
		PHYSFS_close(file);
	}
}

void ModuleFileSystem::writeFile(const char * path, const void * buffer, uint size)
{
	PHYSFS_file* file = PHYSFS_openWrite(path);
	if (file != nullptr)
	{
		PHYSFS_write(file, (const void*)buffer, 1, size);
		PHYSFS_close(file);
	}
}

const char* ModuleFileSystem::getExtension(const char * path)
{
	std::string str = path;

	size_t i = str.rfind('.', str.length());
	if (i != std::string::npos) {
		str = str.substr(i + 1, str.length() - i);
	}

	return str.c_str();
}

const char* ModuleFileSystem::getFileName(const char* path)
{
	const char* test = normalizePath(path);
	std::string str = test;
	size_t i = str.rfind('/', str.length());
	if (i != std::string::npos) {
		str = str.substr(i + 1, str.length() - i);
	}

	return str.c_str();
}

const char * ModuleFileSystem::normalizePath(const char * path)
{
	std::string str = path;

	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == '\\')
			str[i] = '/';
	}

	return str.c_str();
}
