#include "ModuleMeshLoader.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#pragma comment (lib,"Assimp/libx86/assimp.lib")

ModuleMeshLoader::ModuleMeshLoader(bool start_enabled): Module(start_enabled)
{
}

ModuleMeshLoader::~ModuleMeshLoader()
{
}
