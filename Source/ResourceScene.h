#ifndef __RESOURCESCENE_H__
#define __RESOURCESCENE_H__

#include "Resource.h"

class ResourceScene : public Resource
{
public:
	ResourceScene(uint UID, ResType type);
	~ResourceScene();

	static void setImportDefaults(JSON_Value& importSettings);

public:

	std::vector<uint> meshesUID;
	std::vector<uint> animationsUID;
	std::vector<uint> bonesUID;
};

#endif // !__RESOURCESCENE_H__
