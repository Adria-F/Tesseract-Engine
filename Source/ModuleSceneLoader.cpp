#include "Application.h"
#include "ModuleSceneLoader.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ModuleTextures.h"
#include "ModuleMeshes.h"
#include "ModuleAnimations.h"
#include "GameObject.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"

#include "Component.h"
#include "ComponentTransformation.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentAnimation.h"
#include "ComponentBone.h"

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceScene.h"
#include "ResourceAnimation.h"
#include "ResourceBone.h"

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

bool ModuleSceneLoader::importScene(const char* path, uint UID, std::vector<uint>& meshUIDs, std::vector<uint>& animationUIDs, std::vector<uint>& bonesUIDs, std::string& newPath, JSON_Value* meta, bool newMeta)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{		
		//Import all bones
		vector<ResourceBone*> rBones;

		//Import all meshes
		//Changed
		vector<ResourceMesh*> rMeshes = importMeshes(path, scene, meshUIDs, bonesUIDs, rBones, meta, newMeta);

		//Import all animations
		vector<ResourceAnimation*> rAnimations = importAnimations(path, scene, animationUIDs, meta, newMeta);

		//Import all textures
		vector<ResourceMaterial*> rMaterials = importMaterials(path, scene);

		//Map to link resource meshes with component meshes UIDs
		std::map<uint, ComponentMesh*> meshLinker;

		GameObject* fakeScene = new GameObject();
		fakeScene->UID = 0;

		GameObject* rootGO = loadGameObject(scene, scene->mRootNode, rMeshes, rMaterials, fakeScene, meshLinker);
		if (rootGO != nullptr)
		{
			for (int i = 0; i < rBones.size(); i++)
			{
				GameObject* child = rootGO->getChildByName(rBones[i]->GetName());
				if (child != nullptr)
				{
					ComponentBone* bone = (ComponentBone*)child->AddComponent(BONE);
					bone->RUID=rBones[i]->GetUID();

					meshLinker[rBones[i]->boneMeshUID]->componentsBones.push_back(bone->UID);
				}
			}
			if (rAnimations.size() > 0)
			{
				ComponentAnimation* animation = (ComponentAnimation*)rootGO->AddComponent(ANIMATION);
				animation->RUID=rAnimations.front()->GetUID();
			}			

			std::string filename;
			App->fileSystem->splitPath(path, nullptr, &filename, nullptr);
			rootGO->name = filename;			

			fakeScene->childs.push_back(rootGO);

			saveSceneFile(std::to_string(UID).c_str(), fakeScene);
			RELEASE(fakeScene);

			newPath=FBX_FOLDER + std::to_string(UID) + SCENES_EXTENSION;
		}

		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene %s", path);
		return false;
	}
		
	return true;
}

GameObject* ModuleSceneLoader::loadGameObject(const aiScene* scene, aiNode* node, std::vector<ResourceMesh*> meshes, std::vector<ResourceMaterial*> materials, GameObject* fakeScene, std::map<uint, ComponentMesh*>& meshlinker)
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

	GameObject* GO = new GameObject();
	GO->name = name;
	ComponentTransformation* transformation = (ComponentTransformation*)GO->AddComponent(TRANSFORMATION);
	transformation->position = pos;
	transformation->scale = scale;
	transformation->rotation = rot;
	transformation->localMatrix.Set(float4x4::FromTRS(pos, rot, scale));
	GO->boundingBox = AABB({ 0,0,0 }, { 0,0,0 });

	if (node->mNumMeshes > 0)
	{
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

			if (child != GO)
			{
				ComponentTransformation* transformation = (ComponentTransformation*)child->AddComponent(TRANSFORMATION);
				transformation->position = pos;
				transformation->scale = scale;
				transformation->rotation = rot;
				transformation->localMatrix.Set(float4x4::FromTRS(pos, rot, scale));
			}

			ComponentMesh* mesh = (ComponentMesh*)child->AddComponent(componentType::MESH);
			mesh->assignResource(meshes[node->mMeshes[i]]->GetUID());

			meshlinker[mesh->RUID] = mesh;

			if (materials[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex] != nullptr) //Check that material loaded correctly
			{
				ComponentMaterial* material = (ComponentMaterial*)child->AddComponent(MATERIAL);
				material->assignResource(materials[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex]->GetUID());
			}

			if (i > fail_count)
			{
				App->scene_intro->addGameObject(child, GO, fakeScene);
			}
		}
	}
	if (node->mNumChildren > 0)
	{
		for (int i = 0; i < node->mNumChildren; i++)
		{
			GameObject* child = loadGameObject(scene, node->mChildren[i], meshes, materials, fakeScene, meshlinker);
			if (child != nullptr)
			{
				App->scene_intro->addGameObject(child, GO, fakeScene);
			}
		}
	}

	return GO;
}

//Changed
std::vector<ResourceMesh*> ModuleSceneLoader::importMeshes(const char* path, const aiScene* scene, std::vector<uint>& meshUIDs, std::vector<uint>& bonesUIDs, vector<ResourceBone*>& rBones, JSON_Value* meta, bool newMeta)
{
	vector<ResourceMesh*> rMeshes;
	vector<std::string*> meshesNames;
	vector<std::string*> bonesNames;
	std::map<std::string*, uint> meshesUIDs;
	std::map<std::string*, uint> boneUIDs;
	//Changed
	vector<uint> rBonesUIDs;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		//Create Resource Mesh
		std::string fileName;
		App->fileSystem->splitPath(path, nullptr, &fileName, nullptr);

		std::string meshName = (scene->mMeshes[i]->mName.length > 0) ? scene->mMeshes[i]->mName.C_Str() : fileName+"_Mesh";
		meshName = App->resources->getResourceAvailableName(meshName.c_str(), R_MESH); //Get the available name for the mesh

		ResourceMesh* meshResource = nullptr;

		uint UID = App->resources->getResourceUIDFromMeta(path, meshName.c_str());
		if (UID == 0)
			UID = GENERATE_UID();

		std::string exportedFile;

		//Changed
		if (scene->mMeshes[i]->HasBones())
		{
			rBonesUIDs=importBonesFromMesh(path, scene->mMeshes[i], bonesUIDs, rBones, boneUIDs, bonesNames, UID, meta, newMeta);
		}

		if (App->meshes->importRMesh(scene->mMeshes[i], UID, rBonesUIDs, exportedFile)) //Import the mesh
		{
			meshUIDs.push_back(UID);
			std::string* nameAlloc = new std::string(meshName); //Allocate it to keep it through the loop (Cleaned later)
			meshesNames.push_back(nameAlloc); 
			meshesUIDs[nameAlloc] = UID;

			meshResource = (ResourceMesh*)App->resources->AddResource(R_MESH, UID);
			meshResource->name = meshName;
			meshResource->file = path;
			meshResource->exported_file = exportedFile;
		}
		rMeshes.push_back(meshResource); //Add it even if it is nullptr, to keep correct index order
	}
	if (newMeta)
	{
		App->resources->updateMetaUIDsList(path, "bones", boneUIDs, meta);
		App->resources->updateMetaUIDsList(path, "meshes", meshesUIDs, meta);
	}
	else
	{
		App->resources->updateMetaUIDsList(path, "meshes", meshesUIDs);
		App->resources->updateMetaUIDsList(path, "bones", boneUIDs);
	}

	for (std::map<std::string*, uint>::iterator it = boneUIDs.begin(); it != boneUIDs.end(); it++)		//Clean the vector of names
	{
		std::string* aux = (*it).first;
		RELEASE(aux);
	}
	
	int size = meshesNames.size();
	for (int i = 0; i < size; i++) //Clean the vector of names
		RELEASE(meshesNames[i]);
	meshesNames.clear();
	meshesUIDs.clear(); //The allocated names are cleaned with the vector
	
	size = bonesNames.size();
	for (int i = 0; i < size; ++i)
		RELEASE(bonesNames[i]);
	bonesNames.clear();
	boneUIDs.clear(); //The allocated names are cleaned with the vector

	return rMeshes;
}

std::vector<ResourceAnimation*> ModuleSceneLoader::importAnimations(const char* path, const aiScene* scene, std::vector<uint>& animationUIDs, JSON_Value* meta, bool newMeta)
{
	vector<ResourceAnimation*> rAnimations;
	vector<std::string*> animationsNames;
	std::map<std::string*, uint> animationsUIDs;
	for (int i = 0; i < scene->mNumAnimations; i++)
	{
		std::string fileName;
		App->fileSystem->splitPath(path, nullptr, &fileName, nullptr);

		std::string animationName = (scene->mAnimations[i]->mName.length > 0) ? scene->mAnimations[i]->mName.C_Str() : fileName+"_Animation";
		animationName = App->resources->getResourceAvailableName(animationName.c_str(), R_ANIMATION);

		uint UID = App->resources->getResourceUIDFromMeta(path, animationName.c_str());
		if (UID == 0)
			UID = GENERATE_UID();

		std::string exportedFile;
		ResourceAnimation* animationResource = nullptr;
		if (App->animations->importAnimation(scene->mAnimations[i], UID, exportedFile)) //Import animation
		{
			animationUIDs.push_back(UID);
			std::string* nameAlloc = new std::string(animationName); //Allocate it to keep it through the loop (Cleaned later)
			animationsNames.push_back(nameAlloc); 
			animationsUIDs[nameAlloc] = UID;

			animationResource = (ResourceAnimation*)App->resources->AddResource(R_ANIMATION, UID);
			animationResource->name = animationName;
			animationResource->file = path;
			animationResource->exported_file = exportedFile;
		}
		rAnimations.push_back(animationResource);
	}
	if (newMeta)
		App->resources->updateMetaUIDsList(path, "animations", animationsUIDs, meta);
	else
		App->resources->updateMetaUIDsList(path, "animations", animationsUIDs);
	int size = animationsNames.size();
	for (int i = 0; i < size; i++) //Clean the vector of names
		RELEASE(animationsNames[i]);
	animationsNames.clear();
	animationsUIDs.clear(); //The allocated names are cleaned with the vector

	return rAnimations;
}

std::vector<uint> ModuleSceneLoader::importBonesFromMesh(const char * path, const aiMesh* mesh, std::vector<uint>& bonesUIDs, std::vector<ResourceBone*>& rBones, std::map<std::string*, uint>& boneUIDs, std::vector<std::string*> bonesNames, uint meshUID, JSON_Value* meta, bool newMeta)
{
	std::vector<uint> rBonesUID;
		
	for (int j = 0; j < mesh->mNumBones; j++)
	{
		std::string fileName;
		App->fileSystem->splitPath(path, nullptr, &fileName, nullptr);

		aiBone* bone = mesh->mBones[j];
		std::string boneName = (bone->mName.length > 0) ? bone->mName.C_Str() : fileName+"_Bone";		
		App->fileSystem->getAvailableNameFromArray(bonesNames, boneName);

		uint UID = App->resources->getResourceUIDFromMeta(path, boneName.c_str());
		if (UID == 0)
			UID = GENERATE_UID();

		std::string exportedFile;
		ResourceBone* boneResource = nullptr;
		if (App->animations->importBones(bone, UID, exportedFile))
		{
			//TODO what if different meshes with same name
			//modify scene .meta

			bonesUIDs.push_back(UID);
			std::string* nameAlloc = new std::string(boneName); //Allocate it to keep it through the loop (Cleaned later)
			boneUIDs[nameAlloc] = UID;
			bonesNames.push_back(nameAlloc);

			boneResource = (ResourceBone*)App->resources->AddResource(R_BONE, UID);
			boneResource->name = bone->mName.C_Str();
			boneResource->file = path;
			boneResource->exported_file = exportedFile;
			boneResource->boneMeshUID = meshUID;
		}
		if (boneResource != nullptr)
		{
			rBonesUID.push_back(boneResource->UID);
			rBones.push_back(boneResource);
		}
	}

	return rBonesUID;
}

std::vector<ResourceMaterial*> ModuleSceneLoader::importMaterials(const char* path, const aiScene * scene)
{
	vector<ResourceMaterial*> rMaterials;
	if (scene->HasMaterials()) //Need to check embeded textures
	{
		for (int i = 0; i < scene->mNumMaterials; i++)
		{
			if (scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString texturePath;
				aiReturn textureError = scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texturePath);
				if (textureError == aiReturn::aiReturn_SUCCESS)
				{
					std::string full_path = path;
					std::string newPath;	//Useful with resources
					App->fileSystem->splitPath(path, &full_path, nullptr, nullptr);
					full_path += texturePath.C_Str();

					ResourceMaterial* resource = (ResourceMaterial*)App->resources->GetResource(App->fileSystem->manageDroppedFiles(full_path.c_str()));

					rMaterials.push_back(resource);
				}
				else
					rMaterials.push_back(nullptr); //Add it even if it is nullptr, to keep correct index order
			}
			else
			{
				rMaterials.push_back(importColor(path, scene, i));
			}
		}
	}

	return rMaterials;
}

ResourceMaterial* ModuleSceneLoader::importColor(const char* path, const aiScene* scene, int index)
{
	float4 color;
	aiColor3D aiColor(1.f, 1.f, 1.f);
	scene->mMaterials[index]->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
	color = { aiColor.r, aiColor.g, aiColor.b, 1.0f };

	ResourceMaterial* resource = (ResourceMaterial*)App->resources->getColorResource(color.x, color.y, color.z, color.w);
	if (resource != nullptr)
		return resource;

	std::string currentPath;
	if (App->fileSystem->fileExists(MATERIALS_FOLDER))
	{
		currentPath = App->fileSystem->getRealDirectory(MATERIALS_FOLDER);
	}
	else
	{
		std::string file;
		App->fileSystem->splitPath(path, &file, nullptr, nullptr);
		currentPath = file + MATERIALS_FOLDER;
		App->fileSystem->createDirectory(currentPath.c_str());
	}

	resource = (ResourceMaterial*)App->resources->AddResource(R_COLOR, 0);
	resource->color = color;
	//resource->name = App->resources->getResourceAvailableName("Color", R_COLOR);
	resource->setColorName();

	//Create a file for stroing the material
	char* buffer = new char[sizeof(float) * 4];
	float colors[4] = { color.x, color.y, color.z, 1.0f };
	memcpy(buffer, colors, sizeof(float) * 4);
	
	currentPath += resource->name + MATERIAL_EXTENSION;
	App->fileSystem->writeFile(currentPath.c_str(), buffer, sizeof(float) * 4, true);
	RELEASE_ARRAY(buffer);

	resource->file = currentPath;

	//Force the creation of a .meta
	JSON_File* ret = App->JSON_manager->openWriteFile((currentPath +META_EXTENSION).c_str());

	JSON_Value* meta = ret->createValue();
	meta->addUint("UID", resource->UID);
	meta->addInt("last_change", App->fileSystem->getLastTimeChanged(currentPath.c_str()));

	ret->addValue("meta", meta);
	ret->Write();
	App->JSON_manager->closeFile(ret);

	return resource;
}

bool ModuleSceneLoader::saveSceneFile(const char* scene_name, GameObject* fakeRoot)
{
	string path = App->fileSystem->getFullPath(scene_name, (fakeRoot != nullptr)?FBX_FOLDER:nullptr, SCENES_EXTENSION);
	JSON_File* scene = App->JSON_manager->openWriteFile(path.c_str());

	GameObject* rootObject = (fakeRoot != nullptr) ? fakeRoot : App->scene_intro->root;

	bool ret = saveScene(scene, rootObject);

	scene->Write();
	App->JSON_manager->closeFile(scene);

	return ret;
}

bool ModuleSceneLoader::saveVirtualScene()
{
	App->scene_intro->virtualFile = App->JSON_manager->openVirtualFile();

	bool ret = saveScene(App->scene_intro->virtualFile, App->scene_intro->root);

	return ret;
}

bool ModuleSceneLoader::loadSceneFile(const char * scene_name, bool isFBX)
{
	LOG("Loading scene: %s", scene_name);

	if (!isFBX)
	{
		App->scene_intro->newScene();
		App->camera->BBtoLook = AABB({ 0,0,0 }, { 0,0,0 });
	}

	string path = App->fileSystem->getFullPath(scene_name, (isFBX) ? FBX_FOLDER : nullptr, SCENES_EXTENSION);
	JSON_File* scene = App->JSON_manager->openReadFile(path.c_str());
	if (scene != nullptr)
		loadScene(scene);
	else
		return false;

	if (App->inGameMode() == false && !isFBX)
		App->camera->FitCamera(App->scene_intro->root->boundingBox);

	App->JSON_manager->closeFile(scene);

	return true;
}

bool ModuleSceneLoader::loadVirtualScene()
{
	if (App->scene_intro->virtualFile != nullptr)
	{
		App->scene_intro->newScene();
		loadScene(App->scene_intro->virtualFile);

		App->JSON_manager->closeFile(App->scene_intro->virtualFile); //Delete old file

		return true;
	}

	return false;
}

bool ModuleSceneLoader::saveScene(JSON_File* scene, GameObject* rootObject)
{
	JSON_Value* gameObjects = scene->createValue();
	gameObjects->convertToArray();

	for (std::list<GameObject*>::iterator it_ch = rootObject->childs.begin(); it_ch != rootObject->childs.end(); it_ch++)
	{
		(*it_ch)->Save(gameObjects);
	}

	scene->addValue("Game Objects", gameObjects);

	return true;
}

bool ModuleSceneLoader::loadScene(JSON_File* scene)
{
	JSON_Value* gameObjects = scene->getValue("Game Objects"); //It is an array of values
	if (gameObjects->getRapidJSONValue()->IsArray()) //Just make sure
	{
		std::map<uint, uint> LinkerComponents;
		std::map<uint, GameObject*> gameobjects;
		for (int i = 0; i < gameObjects->getRapidJSONValue()->Size(); i++)
		{
			GameObject* GO = new GameObject();
			uint UID = GO->Load(gameObjects->getValueFromArray(i), LinkerComponents);
			gameobjects.insert(std::pair<uint, GameObject*>(UID, GO));
			App->camera->BBtoLook.Enclose(GO->boundingBox);
		}

		for (std::map<uint, GameObject*>::iterator it_go = gameobjects.begin(); it_go != gameobjects.end(); it_go++)
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

		for (std::map<uint, GameObject*>::iterator it_go = gameobjects.begin(); it_go != gameobjects.end(); it_go++)
		{
			ComponentAnimation* animation = (ComponentAnimation*)(*it_go).second->GetComponent(ANIMATION);
			if (animation != nullptr)
			{
				animation->assignResource(animation->RUID);
			}
			ComponentMesh* mesh = (ComponentMesh*)(*it_go).second->GetComponent(MESH);
			if (mesh != nullptr)
			{
				for (int i = 0; i < mesh->componentsBones.size(); i++)
				{
					mesh->componentsBones[i] = LinkerComponents[mesh->componentsBones[i]];
				}
			}
		}
	}

	App->scene_intro->FindCameras(App->scene_intro->root);

	App->renderer3D->CalculateGlobalMatrix(App->scene_intro->root);
	App->scene_intro->root->RecalculateBB();

	App->scene_intro->StartQuadTree();
	
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