#ifndef __RESOURCETEXTURE_H__
#define __RESOURCETEXTURE_H__

#include "Resource.h"

class ResourceTexture :
	public Resource
{
public:

	ResourceTexture(uint UID, ResType type);
	~ResourceTexture();

	static void setImportDefaults(JSON_Value& importSettings);

public:

	//std::string name = "";

	uint width = 0;
	uint height = 0;

	uint GL_id = 0;

};

#endif // !__RESOURCETEXTURE_H__
