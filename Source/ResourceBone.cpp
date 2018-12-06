#include "ResourceBone.h"
#include "Application.h"
#include "ModuleFileSystem.h"

ResourceBone::ResourceBone(uint UID, ResType type) :Resource(UID, type)
{
}

ResourceBone::~ResourceBone()
{
}

bool ResourceBone::LoadInMemory()
{
	LoadBone();

	return true;
}

bool ResourceBone::LoadBone()
{
	return true;
}

bool ResourceBone::UnloadFromMemory()
{
	return true;
}
