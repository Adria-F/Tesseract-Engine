#include "Application.h"
#include "ModuleMeshLoader.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneIntro.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "DevIL\include\il.h"
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

#pragma comment (lib,"Assimp/libx86/assimp.lib")
#pragma comment( lib, "DevIL/libx86/DevIL.lib")
#pragma comment( lib, "DevIL/libx86/ILU.lib")
#pragma comment( lib, "DevIL/libx86/ILUT.lib")

ModuleMeshLoader::ModuleMeshLoader(bool start_enabled): Module(start_enabled)
{
}

bool ModuleMeshLoader::Init(rapidjson::Document& document)
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
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
	bool errorLoading = false;

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			Mesh* newMesh = new Mesh();
			aiMesh* currentMesh= scene->mMeshes[i];

			newMesh->name = currentMesh->mName.C_Str();

			newMesh->num_vertices = currentMesh->mNumVertices;
			newMesh->vertices = new float[newMesh->num_vertices * 3];
			newMesh->num_normals = currentMesh->mNumVertices;
			newMesh->normals = new float[newMesh->num_normals * 3];
	
			memcpy(newMesh->vertices, currentMesh->mVertices, sizeof(float)*newMesh->num_vertices * 3);
			memcpy(newMesh->normals, currentMesh->mNormals, sizeof(float)*newMesh->num_normals * 3);
			LOG("New Mesh with %d vertices\n",newMesh->num_vertices);
			LOG("New Mesh with %d normals\n", newMesh->num_normals);
			LOG("New Mesh with %d faces\n", currentMesh->mNumFaces);


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
				//Remove the name of the mesh from the path and add the image name
				std::string Path = full_path;
				for (int i = Path.size() - 1; i >= 0; i--)
					if (Path[i] == '\\')
						break;
					else
						Path.pop_back();
				Path += path.C_Str();

				newMesh->texture = loadTexture(Path.c_str(), newMesh->width, newMesh->height);
			}
			else
				LOG("Couldn't load the texture from .fbx file");

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

				newMesh->calculateNormals();
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
			return -1;
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
		return -1;
	}

	ilDeleteImages(1, &ilImage);
	glBindTexture(GL_TEXTURE_2D, textureID);

	LOG("Texture creation successful." );

	return textureID;
}
