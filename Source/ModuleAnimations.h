#ifndef __MODULEANIMATIONS_H__
#define __MODULEANIMATIONS_H__

#include "Module.h"

struct aiAnimation;
struct aiBone;
struct ResourceAnimation;
class ResourceBone;

class ModuleAnimations : public Module
{
public:
	ModuleAnimations(bool start_enabled = true);
	~ModuleAnimations();

	bool importAnimation(aiAnimation* animation, uint UID, std::string& newpath);
	bool saveAnimation(ResourceAnimation* animation, uint UID, std::string& newpath);

	bool importBones(aiBone* bone, uint UID, std::string& newpath);
	bool saveBones(ResourceBone* bone, uint UID, std::string& newpath);
};

#endif // !__MODULEANIMATIONS_H__