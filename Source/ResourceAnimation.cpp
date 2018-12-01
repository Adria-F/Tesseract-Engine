#include "ResourceAnimation.h"



ResourceAnimation::ResourceAnimation(uint UID, ResType type) :Resource(UID, type)
{

}


ResourceAnimation::~ResourceAnimation()
{
	RELEASE_ARRAY(bones);
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

Bone::~Bone()
{
	RELEASE_ARRAY(PosKeysValues);
	RELEASE_ARRAY(PosKeysTimes);

	RELEASE_ARRAY(ScaleKeysValues);
	RELEASE_ARRAY(ScaleKeysTimes);

	RELEASE_ARRAY(RotKeysValues);
	RELEASE_ARRAY(RotKeysTimes);
}
