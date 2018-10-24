#include "Application.h"
#include "ModuleSceneLoader.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ModuleTextures.h"
#include "ModuleMeshes.h"
#include "GameObject.h"

#include "Component.h"
#include "ComponentTransformation.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"

//#include "mmgr/mmgr.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

#pragma comment (lib,"Assimp/libx86/assimp.lib")

void CallLog(const char* str, char* userData);

ModuleSceneLoader::ModuleSceneLoader(bool start_enabled) : Module(start_enabled)
{
}

ModuleSceneLoader::~ModuleSceneLoader()
{
}

bool ModuleSceneLoader::Init(rapidjson::Document & document)
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = CallLog;
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleSceneLoader::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

bool ModuleSceneLoader::importFBXScene(const char * path)
{
	App->scene_intro->newScene();

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		App->camera->BBtoLook = new AABB({ 0,0,0 }, { 0,0,0 });
		aiNode* root = scene->mRootNode;
		LoadGameObjects(scene, root, nullptr);

		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", path);

	return false;
}

void ModuleSceneLoader::LoadGameObjects(const aiScene* scene, aiNode* node, GameObject* parent)
{
	bool errorLoading = false;

	if (node != nullptr)
	{
		//Get pos, scale and rotation of the Game Object
		aiVector3D translation;
		aiVector3D scaling;
		aiQuaternion rotation;
		node->mTransformation.Decompose(scaling, rotation, translation);
		vec3 pos(translation.x, translation.y, translation.z);
		vec3 scale(scaling.x, scaling.y, scaling.z);
		Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

		//Create the Game Object with the information
		GameObject* newGameObject = new GameObject(/*pos,scale,rotation*/);

		//Get name and set parent
		newGameObject->name = node->mName.C_Str();
		newGameObject->parent = parent;

		//Add the Game Object to the scene
		App->scene_intro->GameObjects.push_back(newGameObject);

		//Add the Game Object to the Children list of the parent
		if (parent != nullptr)
		{
			parent->childs.push_back(newGameObject);
		}

		// Generate a game object for each mesh
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			//Creating the GameObject
			GameObject* GameObjectFromMesh = nullptr;

			//Creating the mesh for the componentMesh and getting the mesh from assimp
			Mesh* newMesh = new Mesh();
			aiMesh* currentMesh = scene->mMeshes[node->mMeshes[i]];
			newMesh->name = currentMesh->mName.C_Str();

			//Getting mesh information
			newMesh->num_vertices = currentMesh->mNumVertices;

			//Copying Vertices array
			newMesh->vertices = new float[newMesh->num_vertices * 3]; //It is checked below that at least has 1 face, so at elast 3 vertices
			memcpy(newMesh->vertices, currentMesh->mVertices, sizeof(float)*newMesh->num_vertices * 3);

			//Copying Face Normals
			if (currentMesh->HasNormals())
			{
				newMesh->num_normals = currentMesh->mNumVertices;
				newMesh->normals = new float[newMesh->num_normals * 3];
				memcpy(newMesh->normals, currentMesh->mNormals, sizeof(float)*newMesh->num_normals * 3);
			}

			//Loging Info
			LOG("New Mesh with %d vertices", newMesh->num_vertices);
			LOG("New Mesh with %d normals", newMesh->num_normals);
			LOG("New Mesh with %d faces", currentMesh->mNumFaces);

			//Copying texture coords
			if (currentMesh->HasFaces())
			{
				int t = 0;
				if (currentMesh->HasTextureCoords(0))
				{
					newMesh->texCoords = new float[newMesh->num_vertices * 2];
					for (uint q = 0; q < newMesh->num_vertices * 2; q = q + 2)
					{
						newMesh->texCoords[q] = currentMesh->mTextureCoords[0][t].x;
						newMesh->texCoords[q + 1] = currentMesh->mTextureCoords[0][t].y;
						t++;
					}
				}
				else
				{
					LOG("Current mesh has no Texture Coordinates, so will not draw any texture assigned");
				}

				//Copying indices
				newMesh->num_indices = currentMesh->mNumFaces * 3;
				newMesh->indices = new uint[newMesh->num_indices]; // assume each face is a triangle

				for (int j = 0; j < currentMesh->mNumFaces; ++j)
				{
					if (currentMesh->mFaces[j].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
						LOG("WARNING, face normals couldn't be loaded");
						errorLoading = true;
						break;
					}
					else
					{
						memcpy(&newMesh->indices[j * 3], currentMesh->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}

				//Generating Local BoundingBox
				newMesh->boundingBox.SetNegativeInfinity();
				newMesh->boundingBox.Enclose((float3*)currentMesh->mVertices, newMesh->num_vertices);

				//Generating Global BoundingBox
				App->camera->BBtoLook->Enclose(newMesh->boundingBox);

				newMesh->position = pos;
				newMesh->scale = scale;
				newMesh->rotation = rot;

				/*newMesh->position = pos;
				newMesh->scale = scale;
				newMesh->rotation = rot;*/
			}
			else
			{
				LOG("Current mesh has no faces, so will not be loaded");
				errorLoading = true;
			}

			if (!errorLoading)
			{
				newMesh->calculateNormals();
				App->meshes->saveMesh(newMesh);
				newMesh->GenerateBuffer();
				App->renderer3D->pushMesh(newMesh);

				//Add the mesh inside the cilds(>1) or parent(<1)
				if (node->mNumMeshes > 1)
				{
					//Getting mesh information
					GameObjectFromMesh = new GameObject(/*pos,scale,rotation*/);
					GameObjectFromMesh->name = currentMesh->mName.C_Str();
					GameObjectFromMesh->parent = newGameObject;

					ComponentMesh* component;
					component = (ComponentMesh*)GameObjectFromMesh->AddComponent(MESH);
					component->mesh = newMesh;

				}
				else
				{
					ComponentMesh* component;
					component = (ComponentMesh*)newGameObject->AddComponent(MESH);
					component->mesh = newMesh;
				}
			}
			errorLoading = false;
		}
		App->camera->FitCamera(*App->camera->BBtoLook);

		LOG("New GameObject with name %s", newGameObject->name.c_str());

		for (uint i = 0; i < node->mNumChildren; i++)
		{
			if (node->mChildren[i] != nullptr)
			{
				LoadGameObjects(scene, node->mChildren[i], newGameObject);
			}
		}

	}
}

void CallLog(const char* str, char* userData)
{
	//To remove the \n at the end of each line
	//Our log function already does that
	std::string msg = str;
	msg.pop_back();
	msg.pop_back();

	LOG("%s", msg.c_str());
}