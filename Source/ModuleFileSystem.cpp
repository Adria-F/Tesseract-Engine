#include "ModuleFileSystem.h"
#include "Application.h"
#include "ModuleSceneLoader.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
#include "ModuleGUI.h"

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
		ASSETS_FOLDER, LIBRARY_FOLDER, MESHES_FOLDER, TEXTURES_FOLDER, SCENES_FOLDER
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

bool ModuleFileSystem::Start()
{
	//importAssets();
	getAssetsFiles();

	return true;
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

bool ModuleFileSystem::fileExists(const char* path, const char* atDirectory, const char* withExtension)
{
	return PHYSFS_exists(getFullPath(path, atDirectory, withExtension).c_str());
}

uint ModuleFileSystem::readFile(const char * path, char** buffer)
{
	PHYSFS_file* file = PHYSFS_openRead(path);
	if (file != nullptr)
	{
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(file);
		*buffer = new char[size];
		uint read = PHYSFS_read(file, *buffer, 1, size);
		PHYSFS_close(file);

		if (read != size)
		{
			LOG("Error on reading file: Read data size does not match file size - PHYSFS: %s", PHYSFS_getLastError());
			return 0;
		}

		return size;
	}
	LOG("Error opening file for reading - PHYSFS: %d", PHYSFS_getLastError());
	return 0;
}

uint ModuleFileSystem::writeFile(const char * path, const void * buffer, uint size, bool overwrite)
{
	if (!overwrite)
	{
		path = getAvailablePath(path);
		LOG("File with same name already exists - Saved as: %s", path);
	}

	PHYSFS_file* file = PHYSFS_openWrite(path);
	if (file != nullptr)
	{
		uint written = PHYSFS_write(file, (const void*)buffer, 1, size);		
		PHYSFS_close(file);

		if (written != size)
		{
			LOG("Error on writting file: Written data size does not match buffer size - PHYSFS: %s", PHYSFS_getLastError());
			return 0;
		}

		return written;
	}
	LOG("Error opening file for writting - PHYSFS: %d", PHYSFS_getLastError());
	return 0;
}

const char* ModuleFileSystem::getAvailablePath(const char* path)
{
	uint num_version = 1;

	std::string directory;
	std::string filename;
	std::string extension;
	App->fileSystem->splitPath(path, &directory, &filename, &extension);

	std::string full_path = path;
	while (fileExists(full_path.c_str()))
	{
		num_version++;
		full_path = directory + filename + '(' + std::to_string(num_version) + ')' + extension;
	}

	return full_path.c_str();
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
			*filename = str.substr(0, pos_dot);
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

std::string ModuleFileSystem::getFullPath(const char * path, const char * atDirectory, const char * withExtension)
{
	std::string full_path = path;
	if (atDirectory != nullptr || withExtension != nullptr)
		App->fileSystem->splitPath(path, nullptr, &full_path, nullptr);

	if (atDirectory != nullptr)
		full_path = atDirectory + full_path;
	if (withExtension != nullptr)
		full_path += withExtension;

	return full_path.c_str();
}

void ModuleFileSystem::manageDroppedFiles(const char* path)
{
	//Import to the assets folder and convert+import to the library folder. Name will be: name+(num_version)

	std::string extension = path;
	splitPath(path, nullptr, nullptr, &extension);

	if (extension == "fbx" || extension == "FBX")
	{
		App->scene_loader->importFBXScene(path);
	}
	else if (extension == "png" || extension == "dds")
	{
		std::string full_path = path;
		if (PHYSFS_exists(path)) //It means that the path starts from /Game
		{
			full_path = PHYSFS_getRealDir(path);
			full_path += path;
		}
		App->textures->importTexture(full_path.c_str());
		App->textures->loadTexture(path);
	}
	else
	{
		LOG("Unsupported file format");
	}
}

void ModuleFileSystem::importAssets()
{
	importFilesAt(ASSETS_FOLDER);
}

void ModuleFileSystem::importFilesAt(const char * path)
{
	char** files = PHYSFS_enumerateFiles(path);

	for (char **i = files; *i != NULL; i++)
	{		
		std::string currPath = path;
		currPath += *i;
		
		if (PHYSFS_isDirectory(currPath.c_str()))
		{
			currPath += '/';
			importFilesAt(currPath.c_str());
		}
		else
			manageDroppedFiles(currPath.c_str());
	}
}

void ModuleFileSystem::getAssetsFiles()
{
	App->gui->assets->clearElements();
	getFilesAt(ASSETS_FOLDER);
}

void ModuleFileSystem::getFilesAt(const char * path, assetsElement * element)
{
	char** files = PHYSFS_enumerateFiles(path);

	for (char **i = files; *i != NULL; i++)
	{
		assetsElement* newElem = new assetsElement();
		
		std::string currPath = path;
		currPath += *i;
		std::string extension = "";
		splitPath(currPath.c_str(), nullptr, &newElem->name, &extension);
		if (extension.length() > 0)
			newElem->name += '.' + extension;

		if (PHYSFS_isDirectory(currPath.c_str()))
		{
			currPath += '/';
			getFilesAt(currPath.c_str(), newElem);
		}
		else
			newElem->type = assetsElement::elementType::FILE;

		if (element != nullptr)
			element->pushElement(newElem);
		else
			App->gui->assets->pushElement(newElem);
	}
}
