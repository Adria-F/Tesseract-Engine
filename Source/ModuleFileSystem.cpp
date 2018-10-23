#include "ModuleFileSystem.h"
#include "Application.h"
#include "ModuleMeshLoader.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
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
		ASSETS_FOLDER, LIBRARY_FOLDER, MESHES_FOLDER, TEXTURES_FOLDER
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

uint ModuleFileSystem::readFile(const char * path, char** buffer)
{
	PHYSFS_file* file = PHYSFS_openRead(path);
	if (file != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(file);
		*buffer = new char[size];
		PHYSFS_read(file, *buffer, 1, size);
		PHYSFS_close(file);
		return size;
	}
	return 0;
}

uint ModuleFileSystem::writeFile(const char * path, const void * buffer, uint size)
{
	PHYSFS_file* file = PHYSFS_openWrite(path);
	if (file != nullptr)
	{
		uint written = PHYSFS_write(file, (const void*)buffer, 1, size);
		PHYSFS_close(file);
		return written;
	}
	return 0;
}

void ModuleFileSystem::splitPath(const char* full_path, std::string* path, std::string* filename, std::string* extension)
{
	std::string str = normalizePath(full_path);
	uint pos_slash = str.find_last_of('/');
	uint pos_dot = str.find_last_of('.');

	if (path != nullptr)
	{
		if (pos_slash < str.length())
			*path = str.substr(0, pos_slash + 1);
		else
			path->clear();
	}
	if (filename != nullptr)
	{
		if (pos_slash < str.length())
			*filename = str.substr(pos_slash + 1, pos_dot - pos_slash-1);
		else
			*filename = str.substr(0, str.length() - pos_dot);
	}
	if (extension != nullptr)
	{
		if (pos_dot < str.length())
			*extension = str.substr(pos_dot + 1);
		else
			extension->clear();
	}
}

std::string ModuleFileSystem::normalizePath(const char * path)
{
	std::string str = path;

	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == '\\')
			str[i] = '/';
	}

	return str.c_str();
}

void ModuleFileSystem::manageDroppedFiles(const char* path)
{
	std::string extension = path;
	splitPath(path, nullptr, nullptr, &extension);

	if (extension == "fbx" || extension == "FBX")
	{
		App->mesh_loader->ImportFBX(path);
	}
	else if (extension == "png" || extension == "dds")
	{
		App->renderer3D->ChangeMeshTexture(path);
	}
	else
	{
		LOG("Unsupported file format");
	}
}
