#include "ResourceBone.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ComponentMesh.h"

ResourceBone::ResourceBone(uint UID, ResType type) :Resource(UID, type)
{
}

ResourceBone::~ResourceBone()
{
}

bool ResourceBone::LoadInMemory()
{
	LoadBone();

	return true;
}

bool ResourceBone::LoadBone()
{
	bool ret = true;

	//Get the buffer
	char* buffer = nullptr;
	App->fileSystem->readFile(exported_file.c_str(), &buffer);
	char* cursor = buffer;

	uint bytes = sizeof(uint);
	memcpy(&numWeights, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(float) * 3;
	pos = new float[3];
	memcpy(pos, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(float) * 4;
	rot = new float[4];
	memcpy(rot, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(float) * 3;
	scale = new float[3];
	memcpy(scale, cursor, bytes);
	cursor += bytes;

	Offset = float4x4::FromTRS({ pos[0],pos[1],pos[2] }, { rot[0],rot[1],rot[2],rot[3]}, { scale[0],scale[1],scale[2] });

	if (numWeights > 0)
	{
		weights = new Weight[numWeights];

		for (int i = 0; i < numWeights; i++)
		{
			bytes = sizeof(uint);
			memcpy(&weights[i].VertexID, cursor, bytes);
			cursor += bytes;

			bytes = sizeof(float);
			memcpy(&weights[i].weight, cursor, bytes);
			cursor += bytes;
		}
	}

	return true;
}

bool ResourceBone::UnloadFromMemory()
{

	RELEASE_ARRAY(pos);
	RELEASE_ARRAY(rot);
	RELEASE_ARRAY(scale);
	RELEASE_ARRAY(weights);

	return true;
}
