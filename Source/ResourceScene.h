#ifndef __RESOURCESCENE_H__
#define __RESOURCESCENE_H__

#include "Resource.h"
class ResourceScene : public Resource
{
public:
	ResourceScene(uint UID, ResType type);
	~ResourceScene();

	void Save(JSON_Value* component)const;
	void Load(JSON_Value* component);
	static void setImportDefaults(JSON_Value& importSettings);

public:

	JSON_File* scene;
	std::vector<uint> meshesUID;
};

#endif // !__RESOURCESCENE_H__
