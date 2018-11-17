#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ResourceTexture.h"

#include "DevIL\include\il.h"
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

#pragma comment( lib, "DevIL/libx86/DevIL.lib")
#pragma comment( lib, "DevIL/libx86/ILU.lib")
#pragma comment( lib, "DevIL/libx86/ILUT.lib")

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

	LoadResourceTexture();

	return ret;
}

bool ResourceTexture::LoadResourceTexture()
{
	bool ret = false;
	char* buffer = nullptr;
	std::string full_path = exported_file;
	App->fileSystem->splitPath(exported_file.c_str(), nullptr, &full_path, nullptr);

	uint size = App->fileSystem->readFile(App->fileSystem->getFullPath(full_path.c_str(), TEXTURES_FOLDER, TEXTURES_EXTENSION).c_str(), &buffer);

	if (buffer != nullptr && size > 0)
	{
		ILuint ilImage;
		ilGenImages(1, &ilImage);
		ilBindImage(ilImage);

		if (ilLoadL(IL_DDS, (const void*)buffer, size));
		{
			ILinfo ImageInfo;
			iluGetImageInfo(&ImageInfo);

			if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
				iluFlipImage();

			glGenTextures(1, &GL_id);
			glBindTexture(GL_TEXTURE_2D, GL_id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageInfo.Width, ImageInfo.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
			width = ImageInfo.Width;
			height = ImageInfo.Height;

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		ilDeleteImages(1, &ilImage);
		RELEASE_ARRAY(buffer);
		ret = true;
	}
	else
	{
		LOG("Error loading texture: %s", exported_file.c_str());
		ret = false;
	}

	return ret;
}


bool ResourceTexture::UnloadFromMemory()
{
	glDeleteTextures(1, &GL_id);
	return false;
}
