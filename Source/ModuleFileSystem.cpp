#include "ModuleFileSystem.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ModuleSceneLoader.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
#include "ModuleGUI.h"
#include "Resource.h"
#include "ResourceScene.h"

#include "PhysFS\include\physfs.h"
#include <fstream>

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

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
		ASSETS_FOLDER, LIBRARY_FOLDER, SETTINGS_FOLDER, MESHES_FOLDER, ANIMATIONS_FOLDER,BONES_FOLDER, TEXTURES_FOLDER, FBX_FOLDER
	};
	for (uint i = 0; i < PATHS_AMOUNT; ++i)
	{
		if (PHYSFS_exists(mainPaths[i]) == 0)
			PHYSFS_mkdir(mainPaths[i]);
	}

	addPath(ASSETS_FOLDER);
	addPath(LIBRARY_FOLDER);
	addPath(SETTINGS_FOLDER);

	//Add all extra files inside assets to the search path
	addPathOfFilesAt(ASSETS_FOLDER);

	import_delay = 1000.0f;	
}

ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}

bool ModuleFileSystem::Start()
{
	importFilesAt(ASSETS_FOLDER, true);
	import_timer.Start();

	return true;
}

update_status ModuleFileSystem::Update(float dt)
{
	if (import_timer.ReadTime() >= import_delay)
	{
		importFilesAt(ASSETS_FOLDER);
		import_timer.Start();
	}

	return UPDATE_CONTINUE;
}

bool ModuleFileSystem::addPath(const char * path)
{
	if (PHYSFS_mount(path, nullptr, 1) != 0)
	{
		LOG("Added path: %s", path);
		return true;
	}
	LOG("PHYSFS - Error while adding path: %s", PHYSFS_getLastError());
	return false;
}

bool ModuleFileSystem::removePath(const char * path)
{
	if (PHYSFS_unmount(path) != 0)
	{
		LOG("Removed path: %s", path);
		return true;
	}
	LOG("PHYSFS - Error while removing path: %s", PHYSFS_getLastError());
	return false;
}

void ModuleFileSystem::addPathOfFilesAt(const char* path)
{
	std::list<assetsElement*> elements;
	getFilesAt(path, elements);
	for (std::list<assetsElement*>::iterator it_e = elements.begin(); it_e != elements.end(); it_e++)
	{
		if ((*it_e)->type == assetsElement::FOLDER)
		{
			std::string currPath = path;
			if (currPath.size() > 0 && currPath.back() != '/')
				currPath += '/';
			currPath += (*it_e)->name;
			addPath(currPath.c_str());
			addPathOfFilesAt(currPath.c_str());
		}
	}

	std::list<assetsElement*>::iterator it_e;
	it_e = elements.begin();
	while (it_e != elements.end())
	{
		RELEASE(*it_e);
		it_e++;
	}
	elements.clear();
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
		getAvailablePath(path, &full_path);
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

bool ModuleFileSystem::copyFile(const char* src, const char* dest, bool deleteSource)
{
	bool ret = false;

	std::string filename;
	std::string extension;
	App->fileSystem->splitPath(src, nullptr, &filename, &extension);

	std::string destination = dest;
	if (destination.size() > 0 && destination.back() != '/')
		destination += '/';
	destination += filename;
	if (extension.size() > 0)
		destination += '.' + extension;

	if (PHYSFS_isDirectory(src))
	{
		createDirectory(destination.c_str());
		//Copy content
		ret = true;
	}
	else
	{
		ifstream ifs(src, std::ifstream::in | std::ifstream::binary | std::ifstream::ate); //The file is outside search path, so we cannot use PHYSFS

		ifstream::pos_type fileSize = ifs.tellg();
		ifs.seekg(0, std::ifstream::beg);
		uint size = fileSize;
		if (size == -1) //It means file was not found
			return false;
		vector<char> buffer(fileSize);
		ifs.read(buffer.data(), fileSize);

		if (size > 0)
		{
			writeFile(destination.c_str(), buffer.data(), size, true);
			ret = true;
		}
		//RELEASE_ARRAY(buffer);
		if (extension != "meta")
		{
			std::string metaPath = src;
			metaPath += META_EXTENSION;
			copyFile(metaPath.c_str(), dest, deleteSource);

			App->resources->updateMetaLastChange(destination.c_str());
		}
	}
	if (deleteSource)
		deleteFile(src);
	return ret;
}

bool ModuleFileSystem::createDirectory(const char* path)
{
	std::string newPath;
	getAvailablePath(path, &newPath);

	if (PHYSFS_mkdir(newPath.c_str()) != 0)
	{
		addPath(newPath.c_str());
		return true;
	}
	LOG("PHYSFS - Error while creating directory: %s", PHYSFS_getLastError());
	return false;
}

bool ModuleFileSystem::deleteFile(const char* path)
{
	bool wasDirectory = PHYSFS_isDirectory(path);
	if (wasDirectory)
	{
		std::list<assetsElement*> elements;
		getFilesAt(path, elements);
		for (std::list<assetsElement*>::iterator it_e = elements.begin(); it_e != elements.end(); it_e++)
		{
			std::string currPath = path;
			if (currPath.size() > 0 && currPath.back() != '/')
				currPath += '/';
			currPath += (*it_e)->name;
			deleteFile(currPath.c_str());
		}
		std::list<assetsElement*>::iterator it_e;
		it_e = elements.begin();
		while (it_e != elements.end())
		{
			RELEASE(*it_e);
			it_e++;
		}
		elements.clear();
	}
	if (PHYSFS_delete(path) != 0)
	{
		if (wasDirectory)
			removePath(path);
		return true;
	}
	LOG("PHYSFS - Error while deleting file: %s", PHYSFS_getLastError());
	return false;
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
		RELEASE_ARRAY(buffer);
		//TODO Rename .meta
	}
	deleteFile(full_path.c_str());
	return true;
}

uint ModuleFileSystem::getAvailablePath(const char* originalPath, std::string* path)
{
	uint num_version = 0;

	std::string directory;
	std::string filename;
	std::string extension;
	App->fileSystem->splitPath(originalPath, &directory, &filename, &extension);

	std::string newPath = originalPath;
	while (fileExists(newPath.c_str()))
	{
		num_version++;
		newPath = directory + filename + '(' + std::to_string(num_version) + ')' + extension;
	}

	if (path != nullptr)
		*path = newPath;
	return num_version;
}

uint ModuleFileSystem::getAvailableNameFromArray(std::vector<std::string*> names, std::string& name)
{
	uint num_version = 0;

	std::string final_name = name;
	for (int i = 0; i < names.size(); i++)
	{
		if (names[i]->c_str() == final_name)
		{
			num_version++;
			final_name = name + '(' + std::to_string(num_version) + ')';
			i = -1; //Start again
		}
	}
	name = final_name;

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
	std::string directory;
	if (atDirectory != nullptr || withExtension != nullptr)
		App->fileSystem->splitPath(path, &directory, &full_path, nullptr);

	if (atDirectory != nullptr)
		full_path = atDirectory + full_path;
	else
		full_path = directory + full_path;
	if (withExtension != nullptr)
		full_path += withExtension;

	return full_path.c_str();
}

std::string ModuleFileSystem::getRealDirectory(const char* path)
{
	std::string full_path = PHYSFS_getRealDir(path);
	if (full_path.size() > 0 && full_path.back() != '/')
		full_path += '/';
	full_path += path;
	return full_path.c_str();
}

uint ModuleFileSystem::manageDroppedFiles(const char* path)
{
	uint ret = 0;

	std::string filename = path;
	std::string extension = path;
	splitPath(path, nullptr, &filename, &extension);
	filename = filename.c_str();
	if (extension.size() > 0)
		filename += '.' + extension;

	std::string full_path = path;
	if (!fileExists(filename.c_str())) //If it does not exist within the search path, we will make a copy of it into ASSETS_FOLDER
	{
		copyFile(path, ASSETS_FOLDER);
		full_path = ASSETS_FOLDER + filename;
	}
	else //If it does, just get the directory inside search path to use it in other functions
	{
		full_path = PHYSFS_getRealDir(filename.c_str());
		if (full_path.size() > 0 && full_path.back() != '/')
			full_path += '/';
		full_path += filename;
	}

	if (extension == "fbx" || extension == "FBX" || extension == "dae" || extension == "DAE")
	{
		ret = App->resources->ImportFile(full_path.c_str(), R_SCENE);
	}
	else if (extension == "png" || extension == "dds" || extension == "tga" || extension == "jpg")
	{
		ret = App->resources->ImportFile(full_path.c_str(), R_TEXTURE);
	}
	else if (extension == "tesseractMat")
	{
		ret = App->resources->ImportFile(full_path.c_str(), R_COLOR);
	}
	else
	{
		LOG("Unsupported file format");
	}

	return ret;
}

void ModuleFileSystem::importFilesAt(const char* path, bool firstTime)
{
	char** files = PHYSFS_enumerateFiles(path);

	for (char **i = files; *i != NULL; i++)
	{		
		std::string currPath = path;
		currPath += *i;
		
		if (PHYSFS_isDirectory(currPath.c_str()))
		{
			currPath += '/';
			importFilesAt(currPath.c_str(), firstTime);
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
					//Need to delete also the imported file at Library
					JSON_File* metaFile = App->resources->getMeta((dir_path + filename).c_str());
					uint UID = metaFile->getValue("meta")->getUint("UID");
					Resource* res = App->resources->GetResource(UID);
					if (res != nullptr)
					{
						deleteFile(res->GetExportedFile());
						if (res->GetType() == R_SCENE)
						{
							for (int i = 0; i < ((ResourceScene*)res)->meshesUID.size(); i++)
							{
								Resource* mesh = App->resources->GetResource(((ResourceScene*)res)->meshesUID[i]);
								if (mesh != nullptr)
									App->fileSystem->deleteFile(mesh->GetExportedFile());
							}
						}
					}
					App->resources->deleteResource(UID);		
					//TODO if it's an fbx, need to delete all meshes

					App->JSON_manager->closeFile(metaFile);
					deleteFile(currPath.c_str()); //Delete the .meta file	
				}
			}
			else if (extension != "tesseractScene") //Ignore scene files
			{
				int lastChange = getLastTimeChanged(currPath.c_str());
				//If file updated or .meta file does not exist as getMetaLastChange returns 0 & if is the first time than assets are imported (to create all resources)
				if (firstTime || lastChange > getMetaLastChange(currPath.c_str()))
					manageDroppedFiles(currPath.c_str()); //Import it
			}			
		}
	}
}

void ModuleFileSystem::reimportFiles()
{
	deleteMetasAt(ASSETS_FOLDER);
	deleteFile(LIBRARY_FOLDER);

	const char* mainPaths[] = {
		LIBRARY_FOLDER, MESHES_FOLDER, ANIMATIONS_FOLDER, TEXTURES_FOLDER, FBX_FOLDER
	};
	for (uint i = 0; i < 5; ++i)
	{
		if (PHYSFS_exists(mainPaths[i]) == 0)
			PHYSFS_mkdir(mainPaths[i]);
	}

	App->resources->deleteAllResources();
}

void ModuleFileSystem::deleteMetasAt(const char * path)
{
	std::list<assetsElement*> elements;
	getFilesAt(path, elements);
	for (std::list<assetsElement*>::iterator it_e = elements.begin(); it_e != elements.end(); it_e++)
	{
		std::string currPath = path;
		if (currPath.size() > 0 && currPath.back() != '/')
			currPath += '/';
		currPath += (*it_e)->name;
		std::string extension;
		splitPath(currPath.c_str(), nullptr, nullptr, &extension);
		if ((*it_e)->type == assetsElement::FOLDER)
		{
			deleteMetasAt(currPath.c_str());
		}
		else if (extension == "meta")
		{
			std::string dir_path, filename;
			splitPath(currPath.c_str(), &dir_path, &filename, nullptr);
			JSON_File* metaFile = App->resources->getMeta((dir_path + filename).c_str());
			uint UID = metaFile->getValue("meta")->getUint("UID");
			
			App->resources->deleteResource(UID); //Delete the resource
			App->JSON_manager->closeFile(metaFile);
			deleteFile(currPath.c_str()); //Delete the .meta file

		}
	}
	std::list<assetsElement*>::iterator it_e;
	it_e = elements.begin();
	while (it_e != elements.end())
	{
		RELEASE(*it_e);
		it_e++;
	}
	elements.clear();
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

int ModuleFileSystem::getLastTimeChanged(const char* path) const
{
	PHYSFS_Stat stat;
	if (PHYSFS_stat(path, &stat) != 0)
	{
		return stat.modtime;
	}

	return 0;
}

int ModuleFileSystem::getMetaLastChange(const char* path) const
{
	int ret = 0;
	std::string metaPath = path;
	metaPath += META_EXTENSION;
	JSON_File* meta = App->JSON_manager->openReadFile(metaPath.c_str());
	if (meta != nullptr)
	{
		JSON_Value* metaValue = meta->getValue("meta");
		if (metaValue != nullptr)
			ret = metaValue->getInt("last_change");
	}

	App->JSON_manager->closeFile(meta);
	return ret;
}

bool ModuleFileSystem::isFileModified(const char* path) const
{
	return (getLastTimeChanged(path) > getMetaLastChange(path));
}
