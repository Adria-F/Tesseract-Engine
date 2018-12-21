#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ResourceMaterial.h"

#include "DevIL\include\il.h"
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

#pragma comment( lib, "DevIL/libx86/DevIL.lib")
#pragma comment( lib, "DevIL/libx86/ILU.lib")
#pragma comment( lib, "DevIL/libx86/ILUT.lib")

ResourceMaterial::ResourceMaterial(uint UID, ResType type):Resource(UID,type)
{
}

ResourceMaterial::~ResourceMaterial()
{
	UnloadFromMemory();
}

void ResourceMaterial::setImportDefaults(JSON_Value & importSettings)
{
}

bool ResourceMaterial::LoadInMemory()
{
	bool ret = true;

	switch (type)
	{
	case R_TEXTURE:
		LoadResourceTexture();
		break;
	case R_COLOR:
		LoadResourceColor();
		break;
	}

	return ret;
}

bool ResourceMaterial::LoadResourceTexture()
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

bool ResourceMaterial::LoadResourceColor()
{
	bool ret = false;
	char* buffer = nullptr;
	uint size = App->fileSystem->readFile(file.c_str(), &buffer);

	if (buffer != nullptr && size > 0)
	{
		float colors[4];
		memcpy(colors, buffer, size);
		color = { colors[0], colors[1], colors[2], colors[3] };

		RELEASE_ARRAY(buffer);
	}

	return false;
}

void ResourceMaterial::setColorName()
{
	name = "Color";

	std::string r, g, b, a;

	/*int first = color.x;
	int test = (int)(color.x * 10);
	test -= (first * 10);
	r += std::to_string(first) + "," + std::to_string((int)(color.x * 10) - (first * 10));
	first = color.y;
	g += std::to_string(first) + "," + std::to_string((int)(color.y * 10) - (first * 10));
	first = color.z;
	b += std::to_string(first) + "," + std::to_string((int)(color.z * 10) - (first * 10));
	first = color.w;
	a += std::to_string(first) + "," + std::to_string((int)(color.w * 10) - (first * 10));*/

	char num[64];
	sprintf(num, "%.2f", color.x);
	r = num;
	sprintf(num, "%.2f", color.y);
	g = num;
	sprintf(num, "%.2f", color.z);
	b = num;
	sprintf(num, "%.2f", color.w);
	a = num;

	name += '(' + r + '-' + g + '-' + b + '-' + a + ')';
}


bool ResourceMaterial::UnloadFromMemory()
{
	if (type == R_TEXTURE)
		glDeleteTextures(1, &GL_id);
	return false;
}
