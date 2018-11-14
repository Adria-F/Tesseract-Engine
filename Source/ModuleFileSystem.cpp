#include "ModuleFileSystem.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ModuleSceneLoader.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
#include "ModuleGUI.h"
#include "Resource.h"
#include <fstream>

#include "PhysFS\include\physfs.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

ModuleFileSystem::ModuleFileSystem(bool start_enabled) : Module(start_enabled)
{
	//Init PHYSFS before Init to allow other modules to use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(normalizePath(base_path).c_str());
	SDL_free(base_path);

	if (PHYSFS_setWriteDir(".") == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	addPath(".");

	//Create main files if they do not exist and add them to the search path
	const char* mainPaths[] = {
		ASSETS_FOLDER, LIBRARY_FOLDER, MESHES_FOLDER, TEXTURES_FOLDER, FBX_FOLDER, SETTINGS_FOLDER
	};
	for (uint i = 0; i < PATHS_AMOUNT; ++i)
	{
		if (PHYSFS_exists(mainPaths[i]) == 0)
			PHYSFS_mkdir(mainPaths[i]);

		addPath(mainPaths[i]);
	}

	import_delay = 1000.0f;
	first_import = true;	
}

ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}

update_status ModuleFileSystem::Update(float dt)
{
	if (first_import || import_timer.ReadTime() >= import_delay)
	{
		importFilesAt(ASSETS_FOLDER);
		import_timer.Start();
		first_import = false;
	}

	return UPDATE_CONTINUE;
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
		std::string full_path;
		getAvailablePath(path, full_path);
		LOG("File with same name already exists - Saved as: %s", full_path);
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

bool ModuleFileSystem::copyFile(const char* src, const char* dest)
{
	std::ifstream source(src, std::ios::binary);
	if (!source.fail())
	{
		std::ofstream destination(dest, std::ios::binary);
		destination << source.rdbuf();
		return source && destination;
	}
	else
		return false;
}

bool ModuleFileSystem::createDirectory(const char* path)
{
	std::string newPath;
	getAvailablePath(path, newPath);

	return PHYSFS_mkdir(newPath.c_str());
}

bool ModuleFileSystem::deleteFile(const char * path)
{
	return PHYSFS_delete(path) != 0;
}

bool ModuleFileSystem::renameFile(const char* path, const char* name)
{
	std::string full_path = path;

	std::string newPath;
	std::string extension;
	splitPath(full_path.c_str(), &newPath, nullptr, &extension);
	newPath += name;
	if (extension.size() > 0)
		newPath += '.' + extension;
	
	if (PHYSFS_isDirectory(full_path.c_str()))
	{
		createDirectory(newPath.c_str());
	}
	else
	{
		char* buffer = nullptr;
		uint size = readFile(full_path.c_str(), &buffer);
		writeFile(newPath.c_str(), buffer, size, true);
	}
	deleteFile(full_path.c_str());
	return true;
}

uint ModuleFileSystem::getAvailablePath(const char* originalPath, std::string& path)
{
	uint num_version = 0;

	std::string directory;
	std::string filename;
	std::string extension;
	App->fileSystem->splitPath(originalPath, &directory, &filename, &extension);

	path = originalPath;
	while (fileExists(path.c_str()))
	{
		num_version++;
		path = directory + filename + '(' + std::to_string(num_version) + ')' + extension;
	}

	return num_version;
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
	//TODO 1: call resource importer.

	std::string filename = path;
	std::string extension = path;
	splitPath(path, nullptr, &filename, &extension);

	if (extension == "fbx" || extension == "FBX")
	{
		//App->scene_loader->importFBXScene(path);
		App->resources->ImportFile(path, R_SCENE);
		
	}
	else if (extension == "png" || extension == "dds" || extension == "tga")
	{
		std::string full_path = path;
		if (PHYSFS_exists(path)) //It means that the path starts from /Game
		{
			full_path = PHYSFS_getRealDir(path);
			full_path += path;
		}
		else
		{ }
		//App->textures->importTexture(full_path.c_str());
		App->resources->ImportFile(full_path.c_str(), R_TEXTURE);
		App->textures->loadTexture(path);
	}
	else
	{
		LOG("Unsupported file format");
	}
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
		{
			std::string dir_path;
			std::string filename;
			std::string extension;
			splitPath(currPath.c_str(), &dir_path, &filename, &extension);
			if (extension == "meta") //If it is a meta file
			{ 
				if (!fileExists((dir_path + filename).c_str())) //If the corresponding file does not exist
				{
					deleteFile(currPath.c_str()); //Delete the .meta file
					//Need to delete also the imported file at Library
				}
			}
			else if (extension != "tesseractScene") //Ignore scene files
			{
				int lastChange = getLastTimeChanged(currPath.c_str());
				if (lastChange > getMetaLastChange(currPath.c_str())) //If file updated or .meta file does not exist as getMetaLastChange returns 0
					manageDroppedFiles(currPath.c_str()); //Import it
			}			
		}
	}
}

void ModuleFileSystem::getFilesAt(const char * path, std::list<assetsElement*>& elements, const char* exclusiveExtension, const char* ignoreExtension)
{
	char** files = PHYSFS_enumerateFiles(path);

	for (char **i = files; *i != NULL; i++)
	{
		std::string currPath = path;
		if (currPath.size() > 0 && currPath.back() != '/')
			currPath += '/';
		currPath += *i;
		std::string filename;
		std::string extension;
		splitPath(currPath.c_str(), nullptr, &filename, &extension);
		if (extension.length() > 0)
			filename += '.' + extension;

		if (!PHYSFS_isDirectory(currPath.c_str()) && ((exclusiveExtension != nullptr && extension != exclusiveExtension) || (ignoreExtension != nullptr && extension == ignoreExtension)))
			continue;

		assetsElement* newElem = new assetsElement();
		newElem->name = filename;
		if (PHYSFS_isDirectory(currPath.c_str()))
			newElem->type = assetsElement::elementType::FOLDER;
		else
			newElem->type = assetsElement::elementType::FILE;

		elements.push_back(newElem);
	}
}

int ModuleFileSystem::getLastTimeChanged(const char* path)
{
	PHYSFS_Stat stat;
	if (PHYSFS_stat(path, &stat) != 0)
	{
		return stat.modtime;
	}

	return 0;
}

int ModuleFileSystem::getMetaLastChange(const char * path)
{
	std::string metaPath = path;
	metaPath += META_EXTENSION;
	JSON_File* meta = App->JSON_manager->openReadFile(metaPath.c_str());
	if (meta != nullptr)
	{
		int lastChange = meta->getValue("meta")->getInt("last_change");
		App->JSON_manager->closeFile(meta);
		return lastChange;
	}

	return 0;
}
