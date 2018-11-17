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
		App->fileSystem->deleteFile(App->resources->GetResource(meshesUID[i])->GetExportedFile());
		App->resources->deleteResource(meshesUID[i]);
	}
	meshesUID.clear();
}

void ResourceScene::setImportDefaults(JSON_Value & importSettings)
{
}
