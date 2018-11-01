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

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		root_counter++;
		App->camera->BBtoLook = AABB({ 0,0,0 }, { 0,0,0 });
		aiNode* root = scene->mRootNode;
		LoadGameObjects(scene, root, nullptr);
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", path);

	return false;
}

bool ModuleSceneLoader::importFSScene(const char * path)
{

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

		vec pos(translation.x, translation.y, translation.z);
		vec scale(scaling.x, scaling.y, scaling.z);
		Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

		//Create the Game Object with the information
		GameObject* newGameObject = new GameObject(/*pos,scale,rotation*/);
		newGameObject->boundingBox.maxPoint = newGameObject->boundingBox.minPoint = { 0,0,0 };

		//Get name and set parent
		newGameObject->parent = parent;
		if (parent == nullptr)
		{
			newGameObject->name += "_"+to_string(root_counter);
			newGameObject->name = node->mName.C_Str();

			//Add the Game Object to the scene
			App->scene_intro->GameObjects.push_back(newGameObject);
		}
		else
		{			
			newGameObject->name = node->mName.C_Str();
			parent->childs.push_back(newGameObject);
		}

		LOG("New GameObject with name %s", newGameObject->name.c_str());

		for (uint i = 0; i < node->mNumChildren; i++)
		{
			if (node->mChildren[i] != nullptr)
			{
				LoadGameObjects(scene, node->mChildren[i],newGameObject);
			}
		}

		// Generate a game object for each mesh
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			//Creating the mesh for the componentMesh and getting the mesh from assimp
			aiMesh* currentMesh = scene->mMeshes[node->mMeshes[i]];
			Mesh* newMesh = LoadGOMesh(currentMesh);

			if (newMesh!=nullptr)
			{
				Texture* currTexture = nullptr;
				if (scene->HasMaterials())
				{
					aiString path;
					aiReturn textureError = scene->mMaterials[currentMesh->mMaterialIndex]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
					string currentPath = path.C_Str();
					if (textureError == aiReturn::aiReturn_SUCCESS)
					{
						if (App->textures->importTexture(currentPath.c_str()))
							currTexture = App->textures->loadTexture(currentPath.c_str()); //Should check if it exists to not reallocate memory
					}
				}

				newMesh->calculateNormals();
				App->meshes->saveMesh(newMesh);
				newMesh->GenerateBuffer();

				//Add the mesh inside the childs(>1) or parent(<1)
				if (node->mNumMeshes > 1)
				{
					//Getting mesh information
					GameObject* GameObjectFromMesh = new GameObject(/*pos,scale,rotation*/);
					GameObjectFromMesh->name = currentMesh->mName.C_Str();
					GameObjectFromMesh->parent = newGameObject;

					GameObjectFromMesh->boundingBox.maxPoint = GameObjectFromMesh->boundingBox.minPoint = { 0,0,0 };

					//Seting transformation
					ComponentTransformation* transformation;
					transformation = (ComponentTransformation*)GameObjectFromMesh->AddComponent(TRANSFORMATION);
					transformation->position = pos;
					transformation->scale = scale;
					transformation->rotation = rot;

					//Adding Mesh Component
					ComponentMesh* component;
					component = (ComponentMesh*)GameObjectFromMesh->AddComponent(MESH);
					component->mesh = newMesh;

					//Seting the Material
					ComponentTexture* material = (ComponentTexture*)GameObjectFromMesh->AddComponent(MATERIAL);
					material->Material = currTexture;

					//Creating the BoundingBox
					GameObjectFromMesh->boundingBox.SetNegativeInfinity();
					GameObjectFromMesh->boundingBox.Enclose((float3*)currentMesh->mVertices, newMesh->num_vertices);

					//Enclose the Child BoundingBox to the parent's bb
					if (GameObjectFromMesh->parent != nullptr)
					{
						newGameObject->boundingBox.Enclose(GameObjectFromMesh->boundingBox);
						newGameObject->childs.push_back(GameObjectFromMesh);
					}

					App->scene_intro->GameObjects.push_back(GameObjectFromMesh);
				}
				else
				{
					//Seting transformation
					ComponentTransformation*transformation = (ComponentTransformation*)newGameObject->AddComponent(TRANSFORMATION);
					transformation->position = pos;
					transformation->scale = scale;
					transformation->rotation = rot;

					//Adding Mesh Component
					ComponentMesh* component = (ComponentMesh*)newGameObject->AddComponent(MESH);
					component->mesh = newMesh;

					//Seting the Material
					ComponentTexture* material = (ComponentTexture*)newGameObject->AddComponent(MATERIAL);
					material->Material = currTexture;

					//Creating the BoundingBox
					newGameObject->boundingBox.SetNegativeInfinity();
					newGameObject->boundingBox.Enclose((float3*)currentMesh->mVertices, newMesh->num_vertices);
				}
				App->camera->BBtoLook.Enclose(newGameObject->boundingBox);
			}
			errorLoading = false;
		}

		if(parent!=nullptr)
			parent->boundingBox.Enclose(newGameObject->boundingBox);

		App->camera->FitCamera(App->camera->BBtoLook);
	}
}


Mesh* ModuleSceneLoader::LoadGOMesh(aiMesh* currentMesh)
{
	Mesh* newMesh = new Mesh();
	newMesh->name = currentMesh->mName.C_Str();

	if (currentMesh->mNumVertices > 0)
	{
		//Getting mesh information
		newMesh->num_vertices = currentMesh->mNumVertices;

		//Copying Vertices array
		newMesh->vertices = new float[newMesh->num_vertices * 3]; //It is checked below that at least has 1 face, so at least 3 vertices
		memcpy(newMesh->vertices, currentMesh->mVertices, sizeof(float)*newMesh->num_vertices * 3);
	}

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
			newMesh->num_texCoords = currentMesh->mNumVertices;
			newMesh->texCoords = new float[newMesh->num_texCoords * 2];
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
				newMesh = nullptr;
				break;
			}
			else
			{
				memcpy(&newMesh->indices[j * 3], currentMesh->mFaces[j].mIndices, 3 * sizeof(uint));
			}
		}		
	}
	else
	{
		LOG("Current mesh has no faces, so will not be loaded");
		newMesh = nullptr;
	}

	return newMesh;
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
	App->camera->BBtoLook = AABB({ 0,0,0 }, { 0,0,0 });

	JSON_File* scene = App->JSON_manager->openReadFile(App->fileSystem->getFullPath(scene_name, SCENES_FOLDER, SCENES_EXTENSION).c_str());

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