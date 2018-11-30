#include "ResourceAnimation.h"



ResourceAnimation::ResourceAnimation(uint UID, ResType type) :Resource(UID, type)
{

}


ResourceAnimation::~ResourceAnimation()
{
}

void ResourceAnimation::setImportDefaults(JSON_Value& importSettings)
{

}

bool ResourceAnimation::LoadInMemory()
{
	LoadAnimation();

	return true;
}
bool ResourceAnimation::LoadAnimation()
{
	return true;
}
bool ResourceAnimation::UnloadFromMemory()
{
	return true;
}