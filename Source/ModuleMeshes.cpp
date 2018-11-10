#include "ModuleMeshes.h"
#include "Application.h"
#include "Globals.h"
#include "ComponentMesh.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"

#include "Assimp/include/mesh.h"

ModuleMeshes::ModuleMeshes(bool start_enabled): Module(start_enabled)
{
}

Mesh* ModuleMeshes::importMesh(aiMesh* mesh)
{
	Mesh* newMesh = new Mesh();
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
				newMesh = nullptr;
				break;
			}
			else
			{
				memcpy(&newMesh->indices[j * 3], mesh->mFaces[j].mIndices, 3 * sizeof(uint));
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

Mesh* ModuleMeshes::loadMesh(const char* meshName)
{
	Mesh* ret = new Mesh();
	ret->name = meshName;
	std::string path = MESHES_FOLDER;
	path += meshName;
	path += MESH_EXTENSION;

	//Get the buffer
	char* cursor = nullptr;
	App->fileSystem->readFile(path.c_str(), &cursor);

	//Load ranges
	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	ret->num_vertices = ranges[0];
	ret->num_indices = ranges[1];
	ret->num_normals = ranges[2];
	ret->num_texCoords = ranges[3];

	//Load vertices
	if (ret->num_vertices > 0)
	{
		bytes = sizeof(float)*ret->num_vertices * 3;
		ret->vertices = new float[ret->num_vertices * 3];
		memcpy(ret->vertices, cursor, bytes);
		cursor += bytes;
	}

	//Load indices
	if (ret->num_indices > 0)
	{
		bytes = sizeof(uint)*ret->num_indices;
		ret->indices = new uint[ret->num_indices];
		memcpy(ret->indices, cursor, bytes);
		cursor += bytes;
	}

	//Load normals
	if (ret->num_normals > 0)
	{
		bytes = sizeof(float)*ret->num_normals * 3;
		ret->normals = new float[ret->num_normals * 3];
		memcpy(ret->normals, cursor, bytes);
		cursor += bytes;
	}

	//Load tex_coords
	if (ret->num_texCoords > 0)
	{
		bytes = sizeof(float)*ret->num_texCoords * 2;
		ret->texCoords = new float[ret->num_texCoords * 2];
		memcpy(ret->texCoords, cursor, bytes);
		cursor += bytes;
	}

	/*//Load position
	float position[3];
	bytes = sizeof(float) * 3;
	memcpy(position, cursor, bytes);
	ret->position = { position[0],position[1],position[2] };
	cursor += bytes;

	//Load scale
	float scale[3];
	bytes = sizeof(float) * 3;
	memcpy(scale, cursor, bytes);
	ret->scale = { scale[0],scale[1],scale[2] };
	cursor += bytes;

	//Load rotation
	float rotation[4];
	bytes = sizeof(float) * 4;
	memcpy(rotation, cursor, bytes);
	ret->rotation = { rotation[0],rotation[1],rotation[2],rotation[3] };*/

	//Calculate bounding box
	ret->boundingBox.SetNegativeInfinity();
	ret->boundingBox.Enclose((float3*)ret->vertices, ret->num_vertices);

	ret->GenerateBuffer();
	App->renderer3D->meshes.push_back(ret);

	return ret;
}

bool ModuleMeshes::saveMesh(Mesh* mesh, std::string& newpath)
{
	bool ret = true;

	//	Vertices | Indices | Normals(vertices) | Texture coords (verties*2)
	uint ranges[4] = { mesh->num_vertices, mesh->num_indices, mesh->num_normals, mesh->num_texCoords};
	
	//Total size of the buffer
	uint size = sizeof(ranges) + sizeof(float)*mesh->num_vertices*3 + sizeof(uint)*mesh->num_indices + sizeof(float)*mesh->num_normals*3 + sizeof(float)*mesh->num_texCoords * 2;
	//size += sizeof(float) * 10;
	char* buffer = new char[size];
	char* cursor = buffer;

	//Store ranges
	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	//Store vertices
	bytes = sizeof(float)*mesh->num_vertices*3;
	memcpy(cursor, mesh->vertices, bytes);
	cursor += bytes;
	
	//Store indices
	bytes = sizeof(uint)*mesh->num_indices;
	memcpy(cursor, mesh->indices, bytes);
	cursor += bytes;

	//Store normals
	bytes = sizeof(float)*mesh->num_normals*3;
	memcpy(cursor, mesh->normals, bytes);
	cursor += bytes;

	//Store tex_coords
	bytes = sizeof(float)*mesh->num_texCoords*2;
	memcpy(cursor, mesh->texCoords, bytes);	
	cursor += bytes;

	/*//Store position
	float position[3] = { mesh->position.x ,mesh->position.y,mesh->position.z };
	bytes = sizeof(position);
	memcpy(cursor, position, bytes);
	cursor += bytes;

	//Store scale
	float scale[3] = { mesh->scale.x ,mesh->scale.y,mesh->scale.z };
	bytes = sizeof(scale);
	memcpy(cursor, scale, bytes);
	cursor += bytes;

	//Store rotation
	float rotation[4] = { mesh->rotation.x ,mesh->rotation.y,mesh->rotation.z,mesh->rotation.w };
	bytes = sizeof(rotation);
	memcpy(cursor, rotation, bytes);*/

	App->fileSystem->writeFile((MESHES_FOLDER + mesh->name + MESH_EXTENSION).c_str(), buffer, size, true); //Overwrite must be set to false when scenes save/load is completed
	newpath = MESHES_FOLDER + mesh->name + MESH_EXTENSION;

	return ret;
}

