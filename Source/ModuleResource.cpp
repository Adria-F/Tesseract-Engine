#include "Application.h"
#include "ModuleResource.h"
#include "ModuleSceneLoader.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceScene.h"
#include "ResourceTexture.h"

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

	resources.clear();

	return true;
}

uint ModuleResource::Find(const char * file) const
{
	return uint();
}

uint ModuleResource::ImportFile(const char* file, ResType type)
{
	uint ret = 0;
	std::string written_file;
	bool loaded = false;

	std::string path = file;
	if (type == R_TEXTURE)
	{
		std::string filename;
		std::string extension;
		App->fileSystem->splitPath(file, nullptr, &filename, &extension);
		path = ASSETS_FOLDER;
		path += "Textures/";
		path += filename + '.' + extension;
	}

	bool newMeta = false;
	JSON_File* meta = getMeta(path.c_str());
	if (meta == nullptr)
	{	
		meta = createMeta(path.c_str(), type);
		newMeta = true;
	}
	else //If a .meta exists
	{

	}

	switch (type)
	{
	case R_TEXTURE:
		loaded=App->textures->importTexture(file,written_file, meta->getValue("ImportSettings"));
		break;
	case R_SCENE:
		loaded=App->scene_loader->importFBXScene(file, meta->getValue("ImportSettings"));
		break;
	}

	if (loaded)
	{
		Resource* newRes = AddResource(type);
		newRes->file = path.c_str();
		newRes->exported_file = written_file;
		ret = newRes->UID;
		LOG("Source Created");
		if (newMeta)
			meta->Write();
	}

	App->JSON_manager->closeFile(meta);	
	return ret;
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

Resource * ModuleResource::AddResource(ResType type, uint forced_uid)
{
	Resource* ret =nullptr;

	//TEMP
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