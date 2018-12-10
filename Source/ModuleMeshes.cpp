#include "ModuleMeshes.h"
#include "Application.h"
#include "Globals.h"
#include "ComponentMesh.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"

#include "Resource.h"
#include "ResourceMesh.h"

#include "Assimp/include/mesh.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

ModuleMeshes::ModuleMeshes(bool start_enabled): Module(start_enabled)
{
}

//Changed
bool ModuleMeshes::importRMesh(aiMesh* mesh,uint UID, std::vector<uint> rBonesUID, std::string& path)
{
	bool ret = false;
	
	ResourceMesh* newMesh = new ResourceMesh(0,R_MESH);//Create the resource but not call the add method
	newMesh->name = (mesh->mName.length > 0) ? mesh->mName.C_Str() : "Unnamed";

	if (mesh->mNumVertices > 0)
	{
		//Getting mesh information
		newMesh->num_vertices = mesh->mNumVertices;

		//Copying Vertices array
		newMesh->vertices = new float[newMesh->num_vertices * 3]; //It is checked below that at least has 1 face, so at least 3 vertices
		memcpy(newMesh->vertices, mesh->mVertices, sizeof(float)*newMesh->num_vertices * 3);
	}

	//Copying Face Normals
	if (mesh->HasNormals())
	{
		newMesh->num_normals = mesh->mNumVertices;
		newMesh->normals = new float[newMesh->num_normals * 3];
		memcpy(newMesh->normals, mesh->mNormals, sizeof(float)*newMesh->num_normals * 3);
	}

	//Loging Info
	LOG("New Mesh with %d vertices", newMesh->num_vertices);
	LOG("New Mesh with %d normals", newMesh->num_normals);
	LOG("New Mesh with %d faces", mesh->mNumFaces);

	//Copying texture coords
	if (mesh->HasFaces())
	{
		ret = true;
		int t = 0;
		if (mesh->HasTextureCoords(0))
		{
			newMesh->num_texCoords = mesh->mNumVertices;
			newMesh->texCoords = new float[newMesh->num_texCoords * 2];
			for (uint q = 0; q < newMesh->num_vertices * 2; q = q + 2)
			{
				newMesh->texCoords[q] = mesh->mTextureCoords[0][t].x;
				newMesh->texCoords[q + 1] = mesh->mTextureCoords[0][t].y;
				t++;
			}
		}
		else
		{
			LOG("Current mesh has no Texture Coordinates, so will not draw any texture assigned");
		}

		//Copying indices
		newMesh->num_indices = mesh->mNumFaces * 3;
		newMesh->indices = new uint[newMesh->num_indices]; // assume each face is a triangle

		for (int j = 0; j < mesh->mNumFaces; ++j)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
				LOG("WARNING, face normals couldn't be loaded");
				RELEASE(newMesh);
				ret = false;
				break;
			}
			else
			{
				memcpy(&newMesh->indices[j * 3], mesh->mFaces[j].mIndices, 3 * sizeof(uint));
			}
		}

		//Changed
		if(rBonesUID.size()>0)
			newMesh->rBonesUID = rBonesUID;
	}
	else
	{
		LOG("Current mesh has no faces, so will not be loaded");
		ret = false;
	}

	if (ret) //Just if successfully imported mesh
		saveMesh(newMesh,UID,path);

	RELEASE(newMesh);
	
	return ret;
}

bool ModuleMeshes::saveMesh(ResourceMesh* mesh, uint UID, std::string& newpath)
{
	bool ret = true;

	//	Vertices | Indices | Normals(vertices) | Texture coords (verties*2)
	uint ranges[5] = { mesh->num_vertices, mesh->num_indices, mesh->num_normals, mesh->num_texCoords,mesh->rBonesUID.size()};

	//Total size of the buffer
	uint size = sizeof(ranges) + sizeof(float)*mesh->num_vertices * 3 + sizeof(uint)*mesh->num_indices + sizeof(float)*mesh->num_normals * 3 + sizeof(float)*mesh->num_texCoords * 2+sizeof(uint)*mesh->rBonesUID.size();
	char* buffer = new char[size];
	char* cursor = buffer;

	//Store ranges
	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	//Store vertices
	bytes = sizeof(float)*mesh->num_vertices * 3;
	memcpy(cursor, mesh->vertices, bytes);
	cursor += bytes;

	//Store indices
	bytes = sizeof(uint)*mesh->num_indices;
	memcpy(cursor, mesh->indices, bytes);
	cursor += bytes;

	//Store normals
	bytes = sizeof(float)*mesh->num_normals * 3;
	memcpy(cursor, mesh->normals, bytes);
	cursor += bytes;

	//Store tex_coords
	bytes = sizeof(float)*mesh->num_texCoords * 2;
	memcpy(cursor, mesh->texCoords, bytes);
	cursor += bytes;

	
	uint* bonesUID = new uint[mesh->rBonesUID.size()];
	for (int i = 0; i < mesh->rBonesUID.size(); i++)
	{
		bonesUID[i]=mesh->rBonesUID[i];
	}

	bytes = sizeof(uint)*mesh->rBonesUID.size();
	memcpy(cursor, bonesUID, bytes);
	cursor += bytes;

	App->fileSystem->writeFile((MESHES_FOLDER + std::to_string(UID) + MESH_EXTENSION).c_str(), buffer, size, true);
	RELEASE_ARRAY(buffer);
	RELEASE_ARRAY(bonesUID);

	newpath = MESHES_FOLDER + std::to_string(UID) + MESH_EXTENSION;

	return ret;
}

