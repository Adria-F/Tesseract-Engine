#ifndef __RESOURCETEXTURE_H__
#define __RESOURCETEXTURE_H__

#include "Resource.h"

class ResourceTexture :
	public Resource
{
public:
	ResourceTexture(uint UID, ResType type);
	~ResourceTexture();
};

#endif // !__RESOURCETEXTURE_H__
