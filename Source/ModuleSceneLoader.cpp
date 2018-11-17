#include "Application.h"
#include "ModuleSceneLoader.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ModuleTextures.h"
#include "ModuleMeshes.h"
#include "GameObject.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"

#include "Component.h"
#include "ComponentTransformation.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ResourceScene.h"

#include <map>

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

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

bool ModuleSceneLoader::importFBXScene(const char* path, uint UID, std::vector<uint>& UIDs, std::string& newPath, JSON_Value* meta, bool newMeta)
{
	//TODO deactivate this two lines after finished with the function
	App->scene_intro->newScene();
	App->camera->BBtoLook = AABB({ 0,0,0 }, { 0,0,0 });

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		//Import all meshes
		vector<ResourceMesh*> rMeshes;
		vector<std::string*> meshesNames;
		std::map<std::string*, uint> meshesUIDs;
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			//Create Resource Mesh
			std::string meshName = (scene->mMeshes[i]->mName.length > 0) ? scene->mMeshes[i]->mName.C_Str() : "Unnamed";
			App->fileSystem->getAvailableNameFromArray(meshesNames, meshName); //Get the available name for the mesh
			
			ResourceMesh* meshResource = nullptr;

			uint UID = App->resources->getResourceUIDFromMeta(path, meshName.c_str());
			if (UID == 0)
				UID = GENERATE_UID();
			UIDs.push_back(UID);

			std::string exportedFile;
			bool success = App->meshes->importRMesh(scene->mMeshes[i], UID, exportedFile); //Import the mesh
			if (success)
			{
				std::string* nameAlloc = new std::string(meshName);
				meshesNames.push_back(nameAlloc); //Allocate it to keep it through the loop (Cleaned later)
				meshesUIDs[nameAlloc] = UID;

				meshResource = (ResourceMesh*)App->resources->AddResource(R_MESH, UID);
				meshResource->name = meshName;
				meshResource->file = path;
				meshResource->exported_file = exportedFile;

				//TODO move this into resource material
				aiColor3D color(0.f, 0.f, 0.f);
				scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
				meshResource->color = { color.r, color.g, color.b };
			}
			rMeshes.push_back(meshResource); //Add it even if it is nullptr, to keep correct index order
		}
		if(newMeta)
			App->resources->updateMeshesUIDs(path, meshesUIDs, meta);
		else
			App->resources->updateMeshesUIDs(path, meshesUIDs);
		int size = meshesNames.size();
		for (int i = 0; i < size; i++) //Clean the vector of names
			RELEASE(meshesNames[i]);
		meshesNames.clear();
		meshesUIDs.clear(); //The allocated names are cleaned with the vector

		//Import textures
		vector<ResourceTexture*> rtextures;
		if (scene->HasMaterials()) //Need to check embeded textures
		{
			for (int i = 0; i < scene->mNumMaterials; i++)
			{
				aiString texturePath;
				aiReturn textureError = scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texturePath);
				if (textureError == aiReturn::aiReturn_SUCCESS)
				{
					std::string full_path = path;
					std::string newPath;	//Useful with resources
					App->fileSystem->splitPath(path, &full_path, nullptr, nullptr);
					full_path += texturePath.C_Str();
					
					ResourceTexture* resource = (ResourceTexture*)App->resources->GetResource(App->fileSystem->manageDroppedFiles(full_path.c_str()));
			
					rtextures.push_back(resource);
				}
				else
				{
					rtextures.push_back(nullptr); //Add it even if it is nullptr, to keep correct index order
				}
			}
		}

		GameObject* rootGO = loadGameObject(scene, scene->mRootNode, rMeshes, rtextures);
		if (rootGO != nullptr)
		{
			std::string filename;
			App->fileSystem->splitPath(path, nullptr, &filename, nullptr);
			rootGO->name = filename;
			App->scene_intro->addGameObject(rootGO);

			saveScene(std::to_string(UID).c_str(), true);

			newPath=FBX_FOLDER + std::to_string(UID) + SCENES_EXTENSION;

			//App->scene_intro->newScene();
		}

		aiReleaseImport(scene);

		App->scene_intro->AddCamera();

		App->renderer3D->CalculateGlobalMatrix(App->scene_intro->root);
		App->scene_intro->root->RecalculateBB();
		App->scene_intro->StartQuadTree();

	}
	else
	{
		LOG("Error loading scene %s", path);
		return false;
	}
		
	return true;
}

GameObject* ModuleSceneLoader::loadGameObject(const aiScene* scene, aiNode* node, std::vector<ResourceMesh*> meshes, std::vector<ResourceTexture*> textures)
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

			ComponentMesh* mesh = (ComponentMesh*)child->AddComponent(componentType::MESH);
			mesh->assignResource(meshes[node->mMeshes[i]]->GetUID());

			if (textures[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex] != nullptr) //Check that material loaded correctly
			{
				ComponentTexture* material = (ComponentTexture*)child->AddComponent(MATERIAL);
				material->assignResource(textures[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex]->GetUID());
			}

			if (i > fail_count)
			{
				App->scene_intro->addGameObject(child, GO);
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

bool ModuleSceneLoader::saveScene(const char* scene_name, bool isFBX)
{
	string path = App->fileSystem->getFullPath(scene_name, (isFBX)?FBX_FOLDER:nullptr, SCENES_EXTENSION);
	JSON_File* scene = App->JSON_manager->openWriteFile(path.c_str());
	if (scene == nullptr)
		return false;

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


bool ModuleSceneLoader::loadScene(const char* scene_name, bool isFBX)
{
	App->scene_intro->newScene();
	LOG("Loading scene: %s", scene_name);
	App->camera->BBtoLook = AABB({ 0,0,0 }, { 0,0,0 });

	string path = App->fileSystem->getFullPath(scene_name, (isFBX) ? FBX_FOLDER : nullptr, SCENES_EXTENSION);
	JSON_File* scene = App->JSON_manager->openReadFile(path.c_str());
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

		for (std::map<uint, GameObject*>::reverse_iterator it_go = gameobjects.rbegin(); it_go != gameobjects.rend(); it_go++) //Reversed to keep the same order as the original
		{
			if ((*it_go).second->parentUID != 0) //If it has a parent
			{
				GameObject* parent = gameobjects[(*it_go).second->parentUID];
				if (parent != nullptr)
				{
					App->scene_intro->addGameObject((*it_go).second, parent);
					parent->boundingBox.Enclose((*it_go).second->boundingBox);
				}
			}
			else
			{
				App->scene_intro->addGameObject((*it_go).second);
			}
		}
	}

	App->renderer3D->CalculateGlobalMatrix(App->scene_intro->root);
	App->scene_intro->root->RecalculateBB();

	if(App->GameMode==false)
		App->camera->FitCamera(App->scene_intro->root->boundingBox);

	App->scene_intro->StartQuadTree();
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