#include "ModuleTextures.h"
#include "Application.h"
#include "Globals.h"
#include "ModuleFileSystem.h"
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

bool ModuleTextures::importTexture(const char* path)
{
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
		
		if (!success)
		{
			error = ilGetError();
			LOG("Image conversion failed - IL error: %s", iluErrorString(error));
			return false;
		}

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
				App->fileSystem->writeFile((TEXTURES_FOLDER + filename + TEXTURES_EXTENSION).c_str(), data, size);
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
	return true;
}

Texture* ModuleTextures::loadTexture(const char* path)
{
	Texture* ret = new Texture();

	char* buffer = nullptr;
	std::string full_path = path;
	App->fileSystem->splitPath(path, nullptr, &full_path, nullptr);
	full_path = TEXTURES_FOLDER + full_path + TEXTURES_EXTENSION;
	uint size = App->fileSystem->readFile(full_path.c_str(), &buffer);

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

			glGenTextures(1, &ret->id);
			glBindTexture(GL_TEXTURE_2D, ret->id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageInfo.Width, ImageInfo.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
			ret->width = ImageInfo.Width;
			ret->height = ImageInfo.Height;

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		ilDeleteImages(1, &ilImage);
	}
	else
	{
		LOG("Error loading texture: %s", path);
		return 0;
	}

	LOG("Texture creation successful.");
	textures.push_back(ret);

	return ret;
}
