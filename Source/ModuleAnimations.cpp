#include "ModuleAnimations.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Resource.h"
#include "ResourceAnimation.h"

#include "Assimp/include/anim.h"

ModuleAnimations::ModuleAnimations(bool start_enabled) : Module(start_enabled)
{
}

ModuleAnimations::~ModuleAnimations()
{
}

bool ModuleAnimations::importAnimation(aiAnimation* animation, uint UID, std::string & newpath)
{
	return false;
}