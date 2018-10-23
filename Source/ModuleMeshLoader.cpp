#include "Application.h"
#include "Globals.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ModuleMeshLoader.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "Assimp/include/mesh.h"
#include "ModuleFileSystem.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "DevIL\include\il.h"
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

#include "mmgr/mmgr.h"

#pragma comment (lib,"Assimp/libx86/assimp.lib")
#pragma comment( lib, "DevIL/libx86/DevIL.lib")
#pragma comment( lib, "DevIL/libx86/ILU.lib")
#pragma comment( lib, "DevIL/libx86/ILUT.lib")

void CallLog(const char* str, char* userData);

ModuleMeshLoader::ModuleMeshLoader(bool start_enabled): Module(start_enabled)
{
}

bool ModuleMeshLoader::Init(rapidjson::Document& document)
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback=CallLog;
	aiAttachLogStream(&stream);

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

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

	ilShutDown();

	return true;
}

void ModuleMeshLoader::ImportFBX(const char* full_path)
{
	App->scene_intro->newScene();

	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);

	usedPath = "";
	usedTexture = 0;
	usedTextureWidth = 0;
	usedTextureHeight = 0;

	if (scene != nullptr && scene->HasMeshes())
	{
		usedPath = "";
		usedTexture = 0;
		usedTextureWidth = 0;
		usedTextureHeight = 0;
		App->camera->BBtoLook = new AABB({ 0,0,0 }, { 0,0,0 });
		aiNode* root = scene->mRootNode;
		LoadGameObjects(scene,root,nullptr);
		//loadNodeMesh(scene, root, full_path);
		
		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene %s", full_path);
}

void ModuleMeshLoader::LoadGameObjects(const aiScene* scene,aiNode* node, GameObject* parent)
{
	bool errorLoading = false;

	if (node!=nullptr)
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
				saveMesh(newMesh);
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
					component =(ComponentMesh*)GameObjectFromMesh->AddComponent(MESH);
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
				LoadGameObjects(scene,node->mChildren[i], newGameObject);
			}
		}

	}
}

void ModuleMeshLoader::loadNodeMesh(const aiScene* scene, aiNode* node, std::string meshPath)
{
	bool errorLoading = false;

	//Recursive for all node children
	for (int i = 0; i < node->mNumChildren; i++)
		loadNodeMesh(scene, node->mChildren[i], meshPath);

	//Get node transformation
	aiVector3D translation;
	aiVector3D scaling;
	aiQuaternion rotation;
	node->mTransformation.Decompose(scaling, rotation, translation);
	vec3 pos(translation.x, translation.y, translation.z);
	vec3 scale(scaling.x, scaling.y, scaling.z);
	Quat rot(rotation.x, rotation.y, rotation.z, rotation.w);

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		Mesh* newMesh = new Mesh();
		aiMesh* currentMesh = scene->mMeshes[node->mMeshes[i]];
		newMesh->name = currentMesh->mName.C_Str();

		newMesh->num_vertices = currentMesh->mNumVertices;
		newMesh->vertices = new float[newMesh->num_vertices * 3]; //It is checked below that at least has 1 face, so at elast 3 vertices
		memcpy(newMesh->vertices, currentMesh->mVertices, sizeof(float)*newMesh->num_vertices * 3);
		if (currentMesh->HasNormals())
		{
			newMesh->num_normals = currentMesh->mNumVertices;
			newMesh->normals = new float[newMesh->num_normals * 3];
			memcpy(newMesh->normals, currentMesh->mNormals, sizeof(float)*newMesh->num_normals * 3);
		}

		LOG("New Mesh with %d vertices", newMesh->num_vertices);
		LOG("New Mesh with %d normals", newMesh->num_normals);
		LOG("New Mesh with %d faces", currentMesh->mNumFaces);

		if (currentMesh->mMaterialIndex < scene->mNumMaterials)
		{
			aiMaterial* mat = scene->mMaterials[currentMesh->mMaterialIndex];
			aiColor3D color(0.f, 0.f, 0.f);
			mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			newMesh->color.x = color.r;
			newMesh->color.y = color.g;
			newMesh->color.z = color.b;

			aiString path;
			aiReturn textureError = mat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
			if (textureError == aiReturn::aiReturn_SUCCESS)
			{
				string currentPath = path.C_Str();
				if (usedPath != currentPath)
				{
					//Remove the name of the mesh from the path and add the image name
					for (int i = meshPath.size() - 1; i >= 0; i--)
						if (meshPath[i] == '/' || meshPath[i] == '\\')
							break;
						else
							meshPath.pop_back();
					meshPath += currentPath;

					newMesh->texture = loadTexture(meshPath.c_str(), newMesh->texWidth, newMesh->texHeight);
					if (newMesh->texture == 0) //Texture not found at root
					{
						LOG("Texture not found at .fbx root");
						LOG("Looking at Assets/Textures folder");
						meshPath = "Assets/Textures/" + currentPath;
						newMesh->texture = loadTexture(meshPath.c_str(), newMesh->texWidth, newMesh->texHeight);
					}
					if (usedTexture == 0)
					{
						usedTexture = newMesh->texture;
						usedTextureWidth = newMesh->texWidth;
						usedTextureHeight = newMesh->texHeight;
					}
				}
				else
				{
					LOG("Texture already loaded");
					newMesh->texture = usedTexture;
					newMesh->texWidth = usedTextureWidth;
					newMesh->texHeight = usedTextureHeight;
				}
				if (usedPath == "")
					usedPath = currentPath;
			}
			else
				LOG("Couldn't read the texture from .fbx file");
		}
		else
		{
			LOG("Mesh material index is out of scene materials array");
		}

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

			newMesh->boundingBox.SetNegativeInfinity();
			newMesh->boundingBox.Enclose((float3*)currentMesh->mVertices, newMesh->num_vertices);
			App->camera->BBtoLook->Enclose(newMesh->boundingBox);

			newMesh->position = pos;
			newMesh->scale = scale;
			newMesh->rotation = rot;
		}
		else
		{
			LOG("Current mesh has no faces, so will not be loaded");
			errorLoading = true;
		}

		if (!errorLoading)
		{
			newMesh->calculateNormals();
			App->renderer3D->pushMesh(newMesh);
		}
		errorLoading = false;
	}
	App->camera->FitCamera(*App->camera->BBtoLook);

}

GLuint ModuleMeshLoader::loadTexture(const char* path, uint& width, uint& height)
{
	ILuint ilImage;	
	GLuint textureID;

	bool success;
	ILenum error;

	ilGenImages(1, &ilImage);
	ilBindImage(ilImage);

	success = ilLoadImage(path);
									
	if (success)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		if (!success)
		{
			error = ilGetError();
			LOG("Image conversion failed - IL error: %s", iluErrorString(error));
			return 0;
		}

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageInfo.Width, ImageInfo.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		width = ImageInfo.Width;
		height = ImageInfo.Height;
	}
	else
	{
		error = ilGetError();
		LOG("Image load failed - IL error: %s", iluErrorString(error));
		return 0;
	}

	ilDeleteImages(1, &ilImage);
	glBindTexture(GL_TEXTURE_2D, textureID);

	LOG("Texture creation successful." );

	return textureID;
}

Mesh* ModuleMeshLoader::importMesh(aiMesh mesh)
{
	return nullptr;
}

Mesh* ModuleMeshLoader::loadMesh(const char* path)
{
	Mesh* ret = new Mesh();
	App->fileSystem->splitPath(path, nullptr, &ret->name, nullptr);

	//Get the buffer
	char* cursor = nullptr;
	App->fileSystem->readFile(path, &cursor);

	//Load ranges
	uint ranges[2];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	ret->num_vertices = ranges[0];
	ret->num_indices = ranges[1];
	ret->num_normals = ret->num_vertices;

	//Load vertices
	bytes = sizeof(float)*ret->num_vertices;
	ret->vertices = new float[ret->num_vertices];
	memcpy(ret->vertices, cursor, bytes);
	cursor += bytes;

	//Load indices
	bytes = sizeof(uint)*ret->num_indices;
	ret->indices = new uint[ret->num_indices];
	memcpy(ret->indices, cursor, bytes);
	cursor += bytes;

	//Load normals
	bytes = sizeof(float)*ret->num_vertices;
	ret->normals = new float[ret->num_normals];
	memcpy(ret->normals, cursor, bytes);
	cursor += bytes;

	//Load tex_coords
	bytes = sizeof(float)*ret->num_vertices * 2;
	ret->texCoords = new float[ret->num_vertices * 2];
	memcpy(ret->texCoords, cursor, bytes);

	//Calculate bounding box
	ret->boundingBox.SetNegativeInfinity();
	ret->boundingBox.Enclose((float3*)ret->vertices, ret->num_vertices);

	return ret;
}

bool ModuleMeshLoader::saveMesh(Mesh* mesh)
{
	bool ret = true;

	//	Vertices | Indices | Normals(vertices) | Texture coords (verties*2)
	uint ranges[2] = { mesh->num_vertices, mesh->num_indices};
	
	//Total size of the buffer
	uint size = sizeof(ranges) + sizeof(float)*mesh->num_vertices + sizeof(uint)*mesh->num_indices + sizeof(float)*mesh->num_vertices + sizeof(float)*mesh->num_vertices*2;
	char* buffer = new char[size];
	char* cursor = buffer;

	//Store ranges
	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	//Store vertices
	bytes = sizeof(float)*mesh->num_vertices;
	memcpy(cursor, mesh->vertices, bytes);
	cursor += bytes;
	
	//Store indices
	bytes = sizeof(uint)*mesh->num_indices;
	memcpy(cursor, mesh->indices, bytes);
	cursor += bytes;

	//Store normals
	bytes = sizeof(float)*mesh->num_normals;
	memcpy(cursor, mesh->normals, bytes);
	cursor += bytes;

	//Store tex_coords
	bytes = sizeof(float)*mesh->num_vertices*2;
	memcpy(cursor, mesh->texCoords, bytes);

	App->fileSystem->writeFile((MESHES_FOLDER + mesh->name + MESH_EXTENSION).c_str(), buffer, size);

	return ret;
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