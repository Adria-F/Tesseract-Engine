#ifndef __RESOURCEMATERIAL_H__
#define __RESOURCEMATERIAL_H__

#include "Resource.h"

class ResourceMaterial : public Resource
{
public:

	ResourceMaterial(uint UID, ResType type);
	~ResourceMaterial();

	static void setImportDefaults(JSON_Value& importSettings);

	bool LoadInMemory();
	bool UnloadFromMemory();

	bool LoadResourceTexture();
	bool LoadResourceColor();

public:

	uint width = 0;
	uint height = 0;

	uint GL_id = 0;

	float4 color = { 0.8f,0.8f,0.8f, 1.0f };
};

#endif // !__RESOURCEMATERIAL_H__
