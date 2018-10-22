#include "ModuleTextures.h"
#include "Application.h"
#include "Globals.h"
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

Texture* ModuleTextures::loadTexture(const char* path)
{
	ILuint ilImage;
	Texture* ret = new Texture();

	bool success;
	ILenum error;

	ilGenImages(1, &ilImage);
	ilBindImage(ilImage);

	success = ilLoadImage(path);

	if (success)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		if (!success)
		{
			error = ilGetError();
			LOG("Image conversion failed - IL error: %s", iluErrorString(error));
			return 0;
		}

		glGenTextures(1, &ret->id);
		glBindTexture(GL_TEXTURE_2D, ret->id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageInfo.Width, ImageInfo.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		ret->width = ImageInfo.Width;
		ret->height = ImageInfo.Height;
	}
	else
	{
		error = ilGetError();
		LOG("Image load failed - IL error: %s", iluErrorString(error));
		return 0;
	}

	ilDeleteImages(1, &ilImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	LOG("Texture creation successful.");
	textures.push_back(ret);

	return ret;
}
