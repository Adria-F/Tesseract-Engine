#ifndef __RESOURCEBONE_H__
#define __RESOURCEBONE_H__

#include "Resource.h"

class ResourceBone : public Resource
{
public:
	ResourceBone(uint UID, ResType type);
	~ResourceBone();

	bool LoadInMemory();
	bool LoadBone();
	bool UnloadFromMemory();

public:

	//weights
};

#endif // !__RESOURCEBONE_H__