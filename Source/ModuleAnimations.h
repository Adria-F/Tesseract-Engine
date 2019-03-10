#ifndef __MODULEANIMATIONS_H__
#define __MODULEANIMATIONS_H__

#include "Module.h"

struct aiAnimation;
struct aiBone;
class ResourceAnimation;
class ResourceBone;
class ResourceAnimationGraph;

class ModuleAnimations : public Module
{
public:
	ModuleAnimations(bool start_enabled = true);
	~ModuleAnimations();

	bool importAnimation(aiAnimation* animation, uint UID, std::string& newpath);
	bool saveAnimation(ResourceAnimation* animation, uint UID, std::string& newpath);

	bool importBones(aiBone* bone, uint UID, std::string& newpath);
	bool saveBones(ResourceBone* bone, uint UID, std::string& newpath);

	bool importGraph(uint UID, std::string& newpath);
	bool saveGraph(ResourceAnimationGraph* graph, uint UID);
};

#endif // !__MODULEANIMATIONS_H__