#include "Application.h"
#include "ModuleTextures.h"
#include "ResourceTexture.h"



ResourceTexture::ResourceTexture(uint UID, ResType type):Resource(UID,type)
{
}


ResourceTexture::~ResourceTexture()
{
	UnloadFromMemory();
}

void ResourceTexture::setImportDefaults(JSON_Value & importSettings)
{
}

bool ResourceTexture::LoadInMemory()
{
	bool ret = true;

	App->textures->LoadResourceTexture(file.c_str(),this);

	return ret;
}

bool ResourceTexture::UnloadFromMemory()
{
	glDeleteTextures(1, &GL_id);
	return false;
}
