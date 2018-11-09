#include "Application.h"
#include "Resource.h"

Resource::Resource(uint UID, ResType type): UID(UID),type(type)
{
}

ResType Resource::GetType() const
{
	return ResType();
}

uint Resource::GetUID() const
{
	return UID;
}

const char * Resource::GetFile() const
{
	return file.c_str();
}

const char * Resource::GetExportedFile() const
{
	return exported_file.c_str();
}

bool Resource::IsLoaded() const
{
	return (loaded>0);
}

bool Resource::Load() const
{
	return false;
}

uint Resource::GetTimesLoaded() const
{
	return loaded;
}
