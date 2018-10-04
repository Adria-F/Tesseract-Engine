#include "Application.h"
#include "ModuleMeshLoader.h"
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

void ModuleMeshLoader::ImportFBX(const char* full_path)
{
	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);
	bool errorLoading = false;

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			Mesh* newMesh = new Mesh();
			aiMesh* currentMesh= scene->mMeshes[i];

			newMesh->num_vertices = currentMesh->mNumVertices;
			newMesh->vertices = new float[newMesh->num_vertices * 3];
			newMesh->num_normals = currentMesh->mNumVertices;
			newMesh->normals = new float[newMesh->num_normals * 3];

			memcpy(newMesh->vertices, currentMesh->mVertices, sizeof(float)*newMesh->num_vertices * 3);
			memcpy(newMesh->normals, currentMesh->mNormals, sizeof(float)*newMesh->num_normals * 3);
			LOG("New Mesh with %d vertices\n",newMesh->num_vertices);
			LOG("New Mesh with %d normals\n", newMesh->num_normals);

			aiMaterial* mat = scene->mMaterials[currentMesh->mMaterialIndex];
			aiColor3D color(0.f, 0.f, 0.f);
			mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			newMesh->color.x = color.r;
			newMesh->color.y = color.g;
			newMesh->color.z = color.b;

			if (currentMesh->HasFaces())
			{
				newMesh->num_indices = currentMesh->mNumFaces * 3;
				newMesh->indices = new uint[newMesh->num_indices]; // assume each face is a triangle

				for (int j = 0; j < currentMesh->mNumFaces; ++j)
				{
					if (currentMesh->mFaces[j].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
						errorLoading = true;
						break;
					}
					else
					{
						memcpy(&newMesh->indices[j * 3], currentMesh->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}
			}
			else
				errorLoading = true;

			if (!errorLoading)
				App->renderer3D->pushMesh(newMesh);
			errorLoading = false;
		}
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", full_path);
}
