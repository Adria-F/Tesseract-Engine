#include "Application.h"
#include "ModuleResource.h"
#include "ModuleSceneLoader.h"
#include "ModuleTextures.h"

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

uint ModuleResource::ImportFile(const char * file, ResType type)
{
	uint ret = 0;
	std::string written_file;
	bool loaded = false;

	switch (type)
	{
	case R_TEXTURE:
		loaded=App->textures->importTexture(file,written_file);
		break;
	case R_SCENE:
		loaded=App->scene_loader->importFBXScene(file);
		break;
	}

	if (loaded)
	{
		Resource* newRes = AddResource(type);
		newRes->file = file;
		newRes->exported_file = written_file;
		ret = newRes->UID;
		LOG("Source Created");
	}
	
	return ret;
}

uint ModuleResource::GenerateNewUID()
{
	return ++last_UID;
}

Resource * ModuleResource::GetResource(uint uid)
{
	if (resources.find(uid) != resources.end())
	{
		return resources[uid];
	}

	return nullptr;
}

Resource * ModuleResource::AddResource(ResType type, uint forced_uid)
{
	Resource* ret =nullptr;

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
	
	resources.insert(std::pair<uint,Resource*>(generateUID(),ret));

	return ret;
}
