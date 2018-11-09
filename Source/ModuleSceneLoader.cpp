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
			Mesh* mesh = App->meshes->importMesh(scene->mMeshes[i]);
			if (mesh != nullptr)
			{
				aiColor3D color(0.f, 0.f, 0.f);
				scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
				mesh->color = { color.r, color.g, color.b }; //Set mesh color
				
				mesh->GenerateBuffer();
				App->meshes->saveMesh(mesh);
			}
			meshes.push_back(mesh); //Even if it is nullptr, add it to the vector to keep correct indices order
		}

		//Import textures
		vector<Texture*> textures;
		if (scene->HasMaterials()) //Need to check embeded textures
		{
			for (int i = 0; i < scene->mNumMaterials; i++)
			{
				aiString texturePath;
				aiReturn textureError = scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texturePath);
				if (textureError == aiReturn::aiReturn_SUCCESS)
				{
					std::string full_path = path;
					App->fileSystem->splitPath(path, &full_path, nullptr, nullptr);
					full_path += texturePath.C_Str();
					App->textures->importTexture(full_path.c_str());
					textures.push_back(App->textures->loadTexture(texturePath.C_Str())); //Even if it is nullptr, add it to the vector to keep correct indices order
				}
				else
					textures.push_back(nullptr);
			}

		}

		GameObject* rootGO = loadGameObject(scene, scene->mRootNode, meshes, textures);
		if (rootGO != nullptr)
		{
			std::string filename;
			App->fileSystem->splitPath(path, nullptr, &filename, nullptr);
			rootGO->name = filename;
			App->scene_intro->addGameObject(rootGO);
		}

		aiReleaseImport(scene);


		App->renderer3D->CalculateGlobalMatrix(App->scene_intro->root);
		App->scene_intro->root->RecalculateBB();
		App->scene_intro->StartQuadTree();

	}
	else
		LOG("Error loading scene %s", path);

	return false;
}

GameObject* ModuleSceneLoader::loadGameObject(const aiScene* scene, aiNode* node, std::vector<Mesh*> meshes, std::vector<Texture*> textures)
{
	aiVector3D translation;
	aiVector3D scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 pos = { translation.x, translation.y, translation.z };
	float3 scale = { scaling.x, scaling.y, scaling.z };
	Quat rot = Quat(rotation.x, rotation.y, rotation.z, rotation.w);

	std::string name = node->mName.C_Str();
	static const char* transformNodes[5] = {
		"$AssimpFbx$_PreRotation", "$AssimpFbx$_Rotation", "$AssimpFbx$_PostRotation",
		"$AssimpFbx$_Scaling", "$AssimpFbx$_Translation" };

	for (int i = 0; i < 5; i++)
	{
		if (name.find(transformNodes[i]) != string::npos && node->mNumChildren > 0)
		{
			node = node->mChildren[0];

			node->mTransformation.Decompose(scaling, rotation, translation);

			pos += { translation.x, translation.y, translation.z };
			scale = { scale.x*scaling.x, scale.y*scaling.y, scale.z*scaling.z };
			rot = rot * Quat(rotation.x, rotation.y, rotation.z, rotation.w);

			name = node->mName.C_Str();
			i = -1;
		}
	}

	GameObject* GO = nullptr;
	if (node->mNumMeshes > 0)
	{
		GO = new GameObject();
		GO->name = name;

		int fail_count = 0;
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			if (meshes[node->mMeshes[i]] == nullptr) //If the laoded mesh is nullptr, skip it
			{
				fail_count++;
				continue;
			}

			GameObject* child = GO;
			if (i > fail_count)
			{
				child = new GameObject();
				child->name = meshes[node->mMeshes[i]]->name;
			}

			ComponentTransformation* transformation = (ComponentTransformation*)child->AddComponent(TRANSFORMATION);
			transformation->position = pos;
			transformation->scale = scale;
			transformation->rotation = rot;
			transformation->localMatrix.Set(float4x4::FromTRS(pos, rot, scale));

			ComponentMesh* mesh = (ComponentMesh*)child->AddComponent(MESH);
			mesh->mesh = meshes[node->mMeshes[i]];

			if (textures[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex] != nullptr) //Check that material loaded correctly
			{
				ComponentTexture* material = (ComponentTexture*)child->AddComponent(MATERIAL);
				material->Material = textures[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];
			}

			if (i > fail_count)
			{
				App->scene_intro->addGameObject(child, GO);
				//CalculateBB
			}
		}
	}
	if (node->mNumChildren > 0)
	{
		if (GO == nullptr)
		{
			GO = new GameObject();
			GO->name = name;
			ComponentTransformation* transformation = (ComponentTransformation*)GO->AddComponent(TRANSFORMATION);
			transformation->position = pos;
			transformation->scale = scale;
			transformation->rotation = rot;
			transformation->localMatrix.Set(float4x4::FromTRS(pos, rot, scale));
		}
		for (int i = 0; i < node->mNumChildren; i++)
		{
			GameObject* child = loadGameObject(scene, node->mChildren[i], meshes, textures);
			if (child != nullptr)
			{
				App->scene_intro->addGameObject(child, GO);
			}
		}
	}

	return GO;
}

bool ModuleSceneLoader::saveScene(const char* scene_name)
{
	JSON_File* scene = App->JSON_manager->openWriteFile(App->fileSystem->getFullPath(scene_name, SCENES_FOLDER, SCENES_EXTENSION).c_str());

	JSON_Value* gameObjects = scene->createValue();
	gameObjects->convertToArray();

	for (std::list<GameObject*>::iterator it_ch = App->scene_intro->root->childs.begin(); it_ch != App->scene_intro->root->childs.end(); it_ch++)
	{
		(*it_ch)->Save(gameObjects);
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
			App->scene_intro->addGameObject(GO);
			gameobjects.insert(std::pair<uint, GameObject*>(GO->UID, GO));
			App->camera->BBtoLook.Enclose(GO->boundingBox);
		}

		for (std::map<uint, GameObject*>::iterator it_go = gameobjects.begin(); it_go != gameobjects.end(); it_go++)
		{
			if ((*it_go).second->parentUID != 0) //If it has a parent
			{
				GameObject* parent = gameobjects[(*it_go).second->parentUID];
				if (parent != nullptr)
				{
					(*it_go).second->changeParent(parent, false);
					parent->boundingBox.Enclose((*it_go).second->boundingBox);
				}
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