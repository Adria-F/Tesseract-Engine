#include "Application.h"
#include "Resource.h"

Resource::Resource(uint UID, ResType type): UID(UID),type(type)
{
}

ResType Resource::GetType() const
{
	return type;
}

uint Resource::GetUID() const
{
	return UID;
}

const char * Resource::GetName() const
{
	return name.c_str();
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

bool Resource::LoadtoMemory()
{
	bool ret = false;

	if (loaded == 0)
	{
		ret = LoadInMemory();
	}

	loaded++;

	return ret;
}

bool Resource::UnloadMemory()
{
	if(loaded>0)
		loaded -= 1;
	if (loaded == 0)
		UnloadFromMemory();

	return false;
}

uint Resource::GetTimesLoaded() const
{
	return loaded;
}
