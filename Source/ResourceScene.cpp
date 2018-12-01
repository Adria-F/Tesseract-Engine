#include "ResourceScene.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"

ResourceScene::ResourceScene(uint UID, ResType type) : Resource(UID, type)
{
}


ResourceScene::~ResourceScene()
{
	for (int i = 0; i < meshesUID.size(); i++)
	{
		App->resources->deleteResource(meshesUID[i]);
	}
	meshesUID.clear();

	for (int i = 0; i < animationsUID.size(); i++)
	{
		App->resources->deleteResource(animationsUID[i]);
	}
	animationsUID.clear();
}

void ResourceScene::setImportDefaults(JSON_Value & importSettings)
{
}
