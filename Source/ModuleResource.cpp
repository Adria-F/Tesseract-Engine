#include "Application.h"
#include "ModuleResource.h"
#include "ModuleSceneLoader.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceScene.h"
#include "ResourceMaterial.h"
#include "ResourceAnimation.h"
#include "ResourceBone.h"

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
	std::vector<uint> animationsUID;
	std::vector<uint> bonesUID;
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
				loaded = App->scene_loader->importScene(file, UID, meshesUID, animationsUID, bonesUID, written_file, metaValue, newMeta);
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
					((ResourceScene*)newRes)->animationsUID = animationsUID;
					((ResourceScene*)newRes)->bonesUID = bonesUID;
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
				JSON_Value* animations = metaValue->getValue("animations");
				if (animations != nullptr)
				{
					//Create the resource for each animation inside the meta
					for (rapidjson::Value::MemberIterator it_m = animations->getRapidJSONValue()->MemberBegin(); it_m != animations->getRapidJSONValue()->MemberEnd(); it_m++)
					{
						uint animationUID = (*it_m).value.GetUint();
						Resource* meshResource = App->resources->AddResource(R_ANIMATION, animationUID);
						meshResource->name = (*it_m).name.GetString();
						meshResource->file = file;
						meshResource->exported_file = ANIMATIONS_FOLDER + std::to_string(animationUID) + ANIMATION_EXTENSION;
						((ResourceScene*)newRes)->animationsUID.push_back(animationUID); //Add its UID to the scene resource list
					}
				}
				newRes->exported_file = FBX_FOLDER + std::to_string(UID) + SCENES_EXTENSION;
				
				JSON_Value* bones = metaValue->getValue("bones");
				if (bones != nullptr)
				{
					//Create the resource for each animation inside the meta
					for (rapidjson::Value::MemberIterator it_m = bones->getRapidJSONValue()->MemberBegin(); it_m != bones->getRapidJSONValue()->MemberEnd(); it_m++)
					{
						uint boneUID = (*it_m).value.GetUint();
						Resource* boneResource = App->resources->AddResource(R_BONE, boneUID);
						boneResource->name = (*it_m).name.GetString();
						boneResource->file = file;
						boneResource->exported_file = BONES_FOLDER + std::to_string(boneUID) + BONE_EXTENSION;
						((ResourceScene*)newRes)->bonesUID.push_back(boneUID); //Add its UID to the scene resource list
					}
				}
				newRes->exported_file = FBX_FOLDER + std::to_string(UID) + SCENES_EXTENSION;
				break;
			
			/*case R_ANIMATION:
				LOG("animation mesh");
				break;*/
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
		ret = (Resource*)new ResourceMaterial(forced_uid, R_TEXTURE);
		break;
	case R_COLOR:
		ret = (Resource*)new ResourceMaterial(forced_uid, R_COLOR);
		break;
	case R_SCENE:
		ret = (Resource*)new ResourceScene(forced_uid, R_SCENE);
		break;
	case R_ANIMATION:
		ret = (Resource*)new ResourceAnimation(forced_uid, R_ANIMATION);
		break;
	case R_BONE:
		ret = (Resource*)new ResourceBone(forced_uid, R_BONE);
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

void ModuleResource::deleteAllResources()
{
	for (std::map<uint, Resource*>::iterator it_rs = resources.begin(); it_rs != resources.end(); it_rs++)
	{
		RELEASE((*it_rs).second);
	}
	resources.clear();
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

std::string ModuleResource::getResourceAvailableName(const char* name, ResType type)
{
	std::vector<Resource*> resources = getResourcesByType(type);
	std::vector<std::string*> names;

	for (int i = 0; i < resources.size(); i++)
	{
		names.push_back(&resources[i]->name);
	}

	std::string ret = name;
	App->fileSystem->getAvailableNameFromArray(names, ret);

	return ret;
}

Resource * ModuleResource::getColorResource(float r, float g, float b, float a)
{

	for (std::map<uint, Resource*>::iterator it_rs = resources.begin(); it_rs != resources.end(); it_rs++)
	{
		if ((*it_rs).second->GetType() == R_COLOR)
		{
			if (((ResourceMaterial*)(*it_rs).second)->color.x == r &&
				((ResourceMaterial*)(*it_rs).second)->color.y == g &&
				((ResourceMaterial*)(*it_rs).second)->color.z == b &&
				((ResourceMaterial*)(*it_rs).second)->color.w == a)
			{
				return (*it_rs).second;
			}
		}
	}

	return nullptr;
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
	case R_COLOR:
		ResourceMaterial::setImportDefaults(*importSettings);
		break;
	case R_SCENE:
		ResourceScene::setImportDefaults(*importSettings);
		break;
	case R_ANIMATION:
		ResourceAnimation::setImportDefaults(*importSettings);
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
bool ModuleResource::updateMetaUIDsList(const char* path, const char* tag, std::map<std::string*, uint> UIDs, JSON_Value* existingMeta)
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
			meta = readFile->getValue(tag);
		}
		if (meta != nullptr)
		{
			JSON_Value* elements = meta->getValue(tag);
			if (elements == nullptr)
				elements = meta->createValue();

			for (std::map<std::string*, uint>::iterator it_m = UIDs.begin(); it_m != UIDs.end(); it_m++)
			{
				elements->setUint((*it_m).first->c_str(), (*it_m).second);
			}
			meta->setValue(tag, elements);
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
uint ModuleResource::getResourceUIDFromMeta(const char * path, const char* tag, const char * elementName)
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
			if (elementName == nullptr)
				ret = meta->getUint("UID");
			else
				ret = meta->getValue(tag)->getUint(elementName);
		}
	}

	App->JSON_manager->closeFile(file);
	return ret;
}
