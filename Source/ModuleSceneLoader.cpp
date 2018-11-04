#include "Application.h"
#include "ModuleSceneLoader.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ModuleTextures.h"
#include "ModuleMeshes.h"
#include "GameObject.h"
#include "ModuleFileSystem.h"

#include "Component.h"
#include "ComponentTransformation.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"

#include <map>
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

bool ModuleSceneLoader::Init(JSON_File* document)
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = CallLog;
	aiAttachLogStream(&stream);

	root_counter = 0;

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
	App->camera->BBtoLook = AABB({ 0,0,0 }, { 0,0,0 });

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		//Import all meshes
		vector<Mesh*> meshes;
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			meshes.push_back(App->meshes->importMesh(scene->mMeshes[i]));
			meshes[i]->GenerateBuffer();
			App->meshes->saveMesh(meshes[i]);
		}

		//Import textures
		vector<Texture*> textures;
		if (scene->HasMaterials()) //Need to check embeded textures
		{			
			for (int i = 0; i < scene->mNumMaterials; i++)
			{
				aiString path;
				aiReturn textureError = scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
				if (textureError == aiReturn::aiReturn_SUCCESS)
				{
					App->textures->importTexture(path.C_Str());
					textures.push_back(App->textures->loadTexture(path.C_Str()));
				}
			}
				
		}

		GameObject* rootGO = loadGO(scene, scene->mRootNode, meshes, textures);
		std::string filename;
		App->fileSystem->splitPath(path, nullptr, &filename, nullptr);
		rootGO->name = filename;
		App->scene_intro->GameObjects.push_back(rootGO);

		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", path);

	return false;
}

GameObject* ModuleSceneLoader::loadGO(const aiScene* scene, aiNode* node, std::vector<Mesh*> meshes, std::vector<Texture*> textures)
{
	//Get pos, scale and rotation of the node
	aiVector3D translation;
	aiVector3D scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	vec pos(translation.x, translation.y, translation.z);
	vec scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

	GameObject* GO = nullptr;
	if (node->mNumMeshes > 0)
	{
		GO = new GameObject();
		std::string name = (node->mName.length > 0) ? node->mName.C_Str() : "Unnamed";
		GO->name = name;

		for (int i = 0; i < node->mNumMeshes; i++)
		{
			GameObject* child = GO;
			if (node->mNumMeshes > 1)
			{
				child = new GameObject();
				child->name = meshes[node->mMeshes[i]]->name;
			}

			//Create transformation
			ComponentTransformation*transformation = (ComponentTransformation*)child->AddComponent(TRANSFORMATION);
			transformation->position = pos;
			transformation->scale = scale;
			transformation->rotation = rot;
			transformation->localMatrix.Set(float4x4::FromTRS(pos, rot, scale));

			//Create mesh
			ComponentMesh* mesh;
			mesh = (ComponentMesh*)child->AddComponent(MESH);
			mesh->mesh = meshes[node->mMeshes[i]];

			//Create material
			ComponentTexture* material = (ComponentTexture*)child->AddComponent(MATERIAL);
			material->Material = textures[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];

			//Calculate BB
			child->boundingBox.SetNegativeInfinity();
			child->boundingBox.Enclose((float3*)scene->mMeshes[node->mMeshes[i]]->mVertices, meshes[node->mMeshes[i]]->num_vertices);

			if (node->mNumMeshes > 1 && i > 0)
			{
				GO->childs.push_back(child);
				GO->boundingBox.Enclose(child->boundingBox);
			}
		}
	}

	if (node->mNumChildren > 0)
	{
		if (GO == nullptr) //It is nullptr if it had no mesh but has childs (not so probable unless it is the root node)
		{
			GO = new GameObject();
			GO->name = "Unnamed";
			ComponentTransformation*transformation = (ComponentTransformation*)GO->AddComponent(TRANSFORMATION);
			transformation->position = pos;
			transformation->scale = scale;
			transformation->rotation = rot;
			transformation->localMatrix.Set(float4x4::FromTRS(pos, rot, scale));
		}
		for (int i = 0; i < node->mNumChildren; i++)
		{
			GameObject* child = loadGO(scene, node->mChildren[i], meshes, textures);
			GO->boundingBox.Enclose(child->boundingBox);
			GO->childs.push_back(child);
		}
	}

	return GO;
}

bool ModuleSceneLoader::saveScene(const char* scene_name)
{
	JSON_File* scene = App->JSON_manager->openWriteFile(App->fileSystem->getFullPath(scene_name, SCENES_FOLDER, SCENES_EXTENSION).c_str());

	JSON_Value* gameObjects = scene->createValue();
	gameObjects->convertToArray();

	for (int i = 0; i < App->scene_intro->GameObjects.size(); i++)
	{
		App->scene_intro->GameObjects[i]->Save(gameObjects);
	}

	scene->addValue("Game Objects", gameObjects);
	scene->Write();
	App->JSON_manager->closeFile(scene);

	return true;
}

bool ModuleSceneLoader::loadScene(const char* scene_name)
{
	App->scene_intro->newScene();
	LOG("Loading scene: %s", scene_name);
	App->camera->BBtoLook = AABB({ 0,0,0 }, { 0,0,0 });

	JSON_File* scene = App->JSON_manager->openReadFile(App->fileSystem->getFullPath(scene_name, SCENES_FOLDER, SCENES_EXTENSION).c_str());
	if (scene == nullptr)
		return false;

	JSON_Value* gameObjects = scene->getValue("Game Objects"); //It is an array of values
	if (gameObjects->getRapidJSONValue()->IsArray()) //Just make sure
	{
		std::map<uint, GameObject*> gameobjects;
		for (int i = 0; i < gameObjects->getRapidJSONValue()->Size(); i++)
		{
			GameObject* GO = new GameObject();
			GO->Load(gameObjects->getValueFromArray(i));
			gameobjects.insert(std::pair<uint, GameObject*>(GO->UID, GO));
			App->camera->BBtoLook.Enclose(GO->boundingBox);
		}

		for (std::map<uint, GameObject*>::iterator it_go = gameobjects.begin(); it_go != gameobjects.end(); it_go++)
		{
			if ((*it_go).second->parentUID == 0) //If it has no parent, add it to the scene list
				App->scene_intro->GameObjects.push_back((*it_go).second);
			else
			{
				GameObject* parent = gameobjects[(*it_go).second->parentUID];
				(*it_go).second->parent = parent;
				parent->childs.push_back((*it_go).second);
				parent->boundingBox.Enclose((*it_go).second->boundingBox);
			}
		}
	}

	App->camera->FitCamera(App->camera->BBtoLook);
	App->JSON_manager->closeFile(scene);
	return true;
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