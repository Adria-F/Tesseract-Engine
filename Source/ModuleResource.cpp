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

	switch (type)
	{
	case TEXTURE:
		App->textures->importTexture(file);
		break;
	case SCENE:
		App->scene_loader->importFBXScene(file);
		break;
	}

	Resource* newRes = AddResource(type);
	newRes->file = file;
	newRes->exported_file = written_file;
	ret = newRes->UID;

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
	case MESH:
		ret = (Resource*)new ResourceMesh(forced_uid, MESH);
		break;
	case TEXTURE:
		ret = (Resource*)new ResourceTexture(forced_uid, TEXTURE);
		break;
	case SCENE:
		ret = (Resource*)new ResourceScene(forced_uid, SCENE);
		break;
	}
	
	resources.insert(std::pair<uint,Resource*>(generateUID(),ret));

	return ret;
}
