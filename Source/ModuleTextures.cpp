#include "ModuleTextures.h"
#include "Application.h"
#include "Globals.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"

#include "ResourceTexture.h"

#include "DevIL\include\il.h"
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

#pragma comment( lib, "DevIL/libx86/DevIL.lib")
#pragma comment( lib, "DevIL/libx86/ILU.lib")
#pragma comment( lib, "DevIL/libx86/ILUT.lib")

ModuleTextures::ModuleTextures(bool start_enabled) : Module(start_enabled)
{
}

ModuleTextures::~ModuleTextures()
{
}

bool ModuleTextures::Init(JSON_File* document)
{
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return true;
}

bool ModuleTextures::CleanUp()
{
	std::list<Texture*>::iterator it_t;
	it_t = textures.begin();
	while (it_t != textures.end())
	{
		RELEASE((*it_t));
		it_t++;
	}
	textures.clear();

	std::list<ResourceTexture*>::iterator it_rt;
	it_rt = resourcetextures.begin();
	while (it_rt != resourcetextures.end())
	{
		RELEASE((*it_rt));
		it_rt++;
	}
	resourcetextures.clear();

	std::list<Texture*>::iterator it_i;
	it_i = icons.begin();
	while (it_i != icons.end())
	{
		RELEASE((*it_i));
		it_i++;
	}
	icons.clear();

	ilShutDown();
	
	return true;
}

bool ModuleTextures::importTexture(const char* path, uint UID, std::string& newpath, JSON_Value* meta)
{
	ILuint ilImage;
	bool success;
	ILenum error;

	ilGenImages(1, &ilImage);
	ilBindImage(ilImage);

	char* buffer = nullptr;
	uint size = App->fileSystem->readFile(path, &buffer);

	success = ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, size);

	if (success)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		ILuint size;
		ILubyte *data;
		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
		size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
		if (size > 0) {
			data = new ILubyte[size]; // allocate data buffer
			if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
			{
				std::string filename = path;
				App->fileSystem->splitPath(path, nullptr, &filename, nullptr);			
				App->fileSystem->writeFile((TEXTURES_FOLDER + std::to_string(UID) + TEXTURES_EXTENSION).c_str(), data, size, true);
				newpath = TEXTURES_FOLDER + std::to_string(UID) + TEXTURES_EXTENSION;
			}
			RELEASE_ARRAY(data);
		}
	}
	else
	{
		error = ilGetError();
		LOG("Image load failed - IL error: %s", iluErrorString(error));
		return false;
	}

	ilDeleteImages(1, &ilImage);
	RELEASE_ARRAY(buffer);
	return true;
}

Texture * ModuleTextures::loadIcon(const char * path)
{
	Texture* ret = new Texture();
	ILuint ilImage;
	bool success;
	ILenum error;

	ilGenImages(1, &ilImage);
	ilBindImage(ilImage);

	success = ilLoadImage(path);

	if (success)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);

		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!success)
		{
			error = ilGetError();
			LOG("Image conversion failed - IL error: %s", iluErrorString(error));
			return 0;
		}

		glGenTextures(1, &ret->GL_id);
		glBindTexture(GL_TEXTURE_2D, ret->GL_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageInfo.Width, ImageInfo.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		ret->width = ImageInfo.Width;
		ret->height = ImageInfo.Height;

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		LOG("Error loading texture: %s", path);
		return 0;
	}

	ilDeleteImages(1, &ilImage);
	icons.push_back(ret);

	return ret;
}

Texture::~Texture()
{
	glDeleteTextures(1, &GL_id);
}
