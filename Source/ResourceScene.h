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

	JSON_File* scene;

};

#endif // !__RESOURCESCENE_H__
