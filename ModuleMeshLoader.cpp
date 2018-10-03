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

bool ModuleMeshLoader::Init(rapidjson::Document& document)
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleMeshLoader::Start()
{
	return true;
}

update_status ModuleMeshLoader::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleMeshLoader::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleMeshLoader::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleMeshLoader::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

ModuleMeshLoader::~ModuleMeshLoader()
{
}
