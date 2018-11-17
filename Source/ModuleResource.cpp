#include "Application.h"
#include "ModuleResource.h"
#include "ModuleSceneLoader.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceScene.h"
#include "ResourceTexture.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

ModuleResource::ModuleResource(bool start_enabled):Module(start_enabled)
{
}


ModuleResource::~ModuleResource()
{
}

bool ModuleResource::Start()
{
	bool ret = true;
	return true;
}

update_status ModuleResource::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleResource::CleanUp()
{
	bool ret = true;

	std::map <uint, Resource*>::iterator it_r;
	it_r = resources.begin();
	while (it_r != resources.end())
	{
		RELEASE((*it_r).second);
		it_r++;
	}
	resources.clear();

	return true;
}

uint ModuleResource::Find(const char * file) const
{
	return uint();
}

uint ModuleResource::ImportFile(const char* file, ResType type)
{
	std::string written_file;
	std::vector<uint> meshesUID;
	bool loaded = false;

	bool newMeta = false;
	JSON_File* meta = getMeta(file);
	if (meta == nullptr)
	{	
		meta = createMeta(file, type);
		newMeta = true;
	}
	JSON_Value* metaValue = meta->getValue("meta");
	uint UID = 0;
	if (metaValue != nullptr)
	{
		UID = metaValue->getUint("UID");

		if (newMeta || App->fileSystem->isFileModified(file))
		{
			switch (type)
			{
			case R_TEXTURE:
				loaded = App->textures->importTexture(file, UID, written_file, metaValue);
				break;
			case R_SCENE:
				loaded = App->scene_loader->importFBXScene(file, UID, meshesUID, written_file, metaValue, newMeta);
				meta->setValue("meta", metaValue);
				break;
			}

			if (loaded)
			{
				Resource* old = GetResource(UID);
				uint timesLoaded = 0;
				if (old != nullptr)
				{
					timesLoaded = old->GetTimesLoaded();
					deleteResource(UID);
				}			
				Resource* newRes = AddResource(type, UID);
				std::string filename;
				App->fileSystem->splitPath(file, nullptr, &filename, nullptr);
				newRes->name = filename;
				newRes->file = file;
				newRes->exported_file = written_file;

				if (type == R_SCENE)
				{
					((ResourceScene*)newRes)->meshesUID = meshesUID;
				}

				for (int i = 0; i < timesLoaded; i++)
				{
					newRes->LoadtoMemory(); //Load it for each time the existing resource was loaded
				}
				LOG("File imported");

				if (newMeta)
					meta->Write();
				else //If meta is not new, means that the file has been modified, so update meta
					updateMetaLastChange(file);
			}			
		}
		else if (GetResource(UID) == nullptr) //If the file is imported but the resources is not created
		{
			Resource* newRes = AddResource(type, UID);
			std::string filename;
			App->fileSystem->splitPath(file, nullptr, &filename, nullptr);
			newRes->name = filename;
			newRes->file = file;

			switch (type)
			{
			case R_TEXTURE:
				newRes->exported_file = TEXTURES_FOLDER + std::to_string(UID) + TEXTURES_EXTENSION;
				break;
			case R_SCENE:
				JSON_Value* meshes = metaValue->getValue("meshes");
				if (meshes != nullptr)
				{
					//Create the resource for each mesh inside the meta
					for (rapidjson::Value::MemberIterator it_m = meshes->getRapidJSONValue()->MemberBegin(); it_m != meshes->getRapidJSONValue()->MemberEnd(); it_m++)
					{
						uint meshUID = (*it_m).value.GetUint();
						Resource* meshResource = App->resources->AddResource(R_MESH, meshUID);
						meshResource->name = (*it_m).name.GetString();
						meshResource->file = file;
						meshResource->exported_file = MESHES_FOLDER + std::to_string(meshUID) + MESH_EXTENSION;
						((ResourceScene*)newRes)->meshesUID.push_back(meshUID); //Add its UID to the scene resource list
					}
				}
				newRes->exported_file = FBX_FOLDER + std::to_string(UID) + SCENES_EXTENSION;
				break;
			}

			LOG("File was already imported, loading resource");
			loaded = true;
		}
			
	}

	App->JSON_manager->closeFile(meta);	
	if (!loaded && newMeta) //If it is not loaded correctly, remove the new .meta file created as the resource will not exist
	{
		std::string metaPath = file;
		metaPath += META_EXTENSION;
		App->fileSystem->deleteFile(metaPath.c_str());
	}
	return UID;
}

uint ModuleResource::GenerateNewUID()
{
	return ++last_UID;
}

Resource * ModuleResource::GetResource(uint uid)
{
	std::map <uint, Resource* >::iterator it = resources.find(uid);
	
	if (resources.find(uid) != resources.end())
	{
		return resources[uid];
	}

	return nullptr;
}

Resource* ModuleResource::AddResource(ResType type, uint forced_uid)
{
	Resource* ret =nullptr;

	if (forced_uid == 0)
		forced_uid = GENERATE_UID();

	switch (type)
	{
	case NO_TYPE:
		ret = new Resource(forced_uid,NO_TYPE);
		break;
	case R_MESH:
		ret = (Resource*)new ResourceMesh(forced_uid, R_MESH);
		break;
	case R_TEXTURE:
		ret = (Resource*)new ResourceTexture(forced_uid, R_TEXTURE);
		break;
	case R_SCENE:
		ret = (Resource*)new ResourceScene(forced_uid, R_SCENE);
		break;
	}
	
	resources.insert(std::pair<uint,Resource*>(forced_uid,ret));

	return ret;
}

bool ModuleResource::deleteResource(uint uid)
{
	if (resources.find(uid) != resources.end())
	{
		Resource* res = resources[uid];
		resources.erase(uid);
		RELEASE(res);

		return true;
	}
	return false;
}

uint ModuleResource::GetResourceByFile(const char* file)
{
	uint ret = 0;

	for (std::map<uint, Resource*>::iterator it_rs = resources.begin(); it_rs != resources.end(); it_rs++)
	{
		if ((*it_rs).second->file == file)
		{
			ret = (*it_rs).second->UID;
			(*it_rs).second->LoadInMemory();
		}
	}

	return ret;
}

std::vector<Resource*> ModuleResource::getResourcesByType(ResType type)
{
	std::vector<Resource*> ret;

	for (std::map<uint, Resource*>::iterator it_rs = resources.begin(); it_rs != resources.end(); it_rs++)
	{
		if ((*it_rs).second->GetType() == type)
			ret.push_back((*it_rs).second);
	}

	return ret;
}

JSON_File* ModuleResource::getMeta(const char* path) const
{
	std::string metaPath = path;
	metaPath += META_EXTENSION;
	JSON_File* ret = App->JSON_manager->openReadFile(metaPath.c_str());

	return ret;
}

JSON_File* ModuleResource::createMeta(const char* path, ResType type) const
{
	std::string metaPath = path;
	metaPath += META_EXTENSION;
	JSON_File* ret = App->JSON_manager->openWriteFile(metaPath.c_str());
	
	JSON_Value* meta = ret->createValue();
	meta->addUint("UID", GENERATE_UID());
	meta->addInt("last_change", App->fileSystem->getLastTimeChanged(path));
	
	JSON_Value* importSettings = meta->createValue();
	switch (type)
	{
	case R_MESH:
		ResourceMesh::setImportDefaults(*importSettings);
		break;
	case R_TEXTURE:
		ResourceTexture::setImportDefaults(*importSettings);
		break;
	case R_SCENE:
		ResourceScene::setImportDefaults(*importSettings);
		break;
	}
	
	meta->addValue("ImportSettings", importSettings);
	ret->addValue("meta", meta);

	return ret;
}

bool ModuleResource::updateMetaLastChange(const char* path)
{
	bool ret = false;
	std::string metaPath = path;
	metaPath += META_EXTENSION;
	JSON_File* readFile = App->JSON_manager->openReadFile(metaPath.c_str());
	if (readFile != nullptr)
	{
		JSON_File* writeFile = App->JSON_manager->openWriteFile(metaPath.c_str());
		JSON_Value* meta = readFile->getValue("meta");
		if (meta != nullptr)
		{
			meta->setUint("last_change", App->fileSystem->getLastTimeChanged(path));
			writeFile->addValue("meta", meta);

			writeFile->Write();
		}
		App->JSON_manager->closeFile(writeFile);
		ret = true;
	}
	App->JSON_manager->closeFile(readFile);
	return ret;
}
bool ModuleResource::updateMeshesUIDs(const char* path, std::map<std::string*, uint> meshesUIDs, JSON_Value* existingMeta)
{
	bool ret = false;
	std::string metaPath = path;
	metaPath += META_EXTENSION;
	JSON_File* readFile = App->JSON_manager->openReadFile(metaPath.c_str());
	if (readFile != nullptr)
	{
		JSON_File* writeFile = nullptr;
		JSON_Value* meta = nullptr;
		if (existingMeta != nullptr)
			meta = existingMeta;
		else
		{
			writeFile = App->JSON_manager->openWriteFile(metaPath.c_str());
			meta = readFile->getValue("meta");
		}
		if (meta != nullptr)
		{
			JSON_Value* meshes = meta->getValue("meshes");
			if (meshes == nullptr)
				meshes = meta->createValue();

			for (std::map<std::string*, uint>::iterator it_m = meshesUIDs.begin(); it_m != meshesUIDs.end(); it_m++)
			{
				meshes->setUint((*it_m).first->c_str(), (*it_m).second);
			}
			meta->setValue("meshes", meshes);
			if (writeFile != nullptr)
			{
				writeFile->addValue("meta", meta);
				writeFile->Write();
			}
		}
		if (writeFile != nullptr)
			App->JSON_manager->closeFile(writeFile);
		ret = true;
	}
	App->JSON_manager->closeFile(readFile);
	return ret;
}
uint ModuleResource::getResourceUIDFromMeta(const char * path, const char * meshName)
{
	uint ret = 0;
	std::string metaPath = path;
	metaPath += META_EXTENSION;
	JSON_File* file = App->JSON_manager->openReadFile(metaPath.c_str());
	if (file != nullptr)
	{
		JSON_Value* meta = file->getValue("meta");
		if (meta != nullptr)
		{
			if (meshName == nullptr)
				ret = meta->getUint("UID");
			else
				ret = meta->getValue("meshes")->getUint(meshName);
		}
	}

	App->JSON_manager->closeFile(file);
	return ret;
}
