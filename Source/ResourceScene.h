#ifndef __RESOURCESCENE_H__
#define __RESOURCESCENE_H__

#include "Resource.h"
class ResourceScene : public Resource
{
public:
	ResourceScene(uint UID, ResType type);
	~ResourceScene();
};

#endif // !__RESOURCESCENE_H__
