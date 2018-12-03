#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceAnimation.h"

#include "Primitive.h"

ResourceAnimation::ResourceAnimation(uint UID, ResType type) :Resource(UID, type)
{

}


ResourceAnimation::~ResourceAnimation()
{
	RELEASE_ARRAY(bones);
}

void ResourceAnimation::setImportDefaults(JSON_Value& importSettings)
{

}

bool ResourceAnimation::LoadInMemory()
{
	LoadAnimation();

	return true;
}

bool ResourceAnimation::LoadAnimation()
{
	bool ret = false;

	//Get the buffer
	char* buffer = nullptr;
	App->fileSystem->readFile(exported_file.c_str(), &buffer);
	char* cursor = buffer;

	//Load ranges
	uint ranges[3];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);
	cursor += bytes;

	time = ranges[0];
	ticksXsecond = ranges[1];
	numBones = ranges[2];

	if (numBones > 0)
	{
		bones = new Bone[numBones];
		spheres = new MSphere[numBones];
		lines = new MLine[numBones];

		//Loading Bones
		for (int i = 0; i < numBones; i++)
		{
			spheres[i].color = { 1.0,0.0f,0.0f,1.0f };
			lines[i].color = { 1.0,0.0f,0.0f,1.0f };

			//Loading Ranges
			uint boneRanges[4];
			uint bytes = sizeof(boneRanges);
			memcpy(boneRanges, cursor, bytes);
			cursor += bytes;

			bones[i].numPosKeys = boneRanges[0];
			bones[i].numScaleKeys = boneRanges[1];
			bones[i].numRotKeys = boneRanges[2];

			//Loading Name
			bytes = boneRanges[3];
			char* auxName = new char[bytes];
			memcpy(auxName, cursor, bytes);
			bones[i].NodeName = auxName;
			bones[i].NodeName = bones[i].NodeName.substr(0, bytes);
			RELEASE_ARRAY(auxName);
			cursor += bytes;

			//Loading Pos Time
			bytes = bones[i].numPosKeys * sizeof(double);
			bones[i].PosKeysTimes = new double[bones[i].numPosKeys];
			memcpy(bones[i].PosKeysTimes, cursor, bytes);
			cursor += bytes;
			//Loading Pos Values
			bytes = bones[i].numPosKeys * sizeof(float)*3;
			bones[i].PosKeysValues = new float[bones[i].numPosKeys*3];
			memcpy(bones[i].PosKeysValues, cursor, bytes);
			cursor += bytes;
			
			//Loading Scale Time
			bytes = bones[i].numScaleKeys * sizeof(double);
			bones[i].ScaleKeysTimes = new double[bones[i].numScaleKeys];
			memcpy(bones[i].ScaleKeysTimes, cursor, bytes);
			cursor += bytes;
			//Loading Scale Values
			bytes = bones[i].numScaleKeys * sizeof(float) * 3;
			bones[i].ScaleKeysValues = new float[bones[i].numScaleKeys * 3];
			memcpy(bones[i].ScaleKeysValues, cursor, bytes);
			cursor += bytes;
			
			//Loading Rotation Time
			bytes = bones[i].numRotKeys * sizeof(double);
			bones[i].RotKeysTimes = new double[bones[i].numRotKeys];
			memcpy(bones[i].RotKeysTimes, cursor, bytes);
			cursor += bytes;
			//Loading Rotation Values
			bytes = bones[i].numRotKeys * sizeof(float) * 4;
			bones[i].RotKeysValues = new float[bones[i].numRotKeys * 4];
			memcpy(bones[i].RotKeysValues, cursor, bytes);
			cursor += bytes;
		}

		ret = true;
	}

	RELEASE_ARRAY(buffer);

	return ret;
}
bool ResourceAnimation::UnloadFromMemory()
{
	return true;
}

void ResourceAnimation::resetFrames()
{
	for (int i = 0; i < numBones; i++)
	{
		bones[i].currentPosIndex = 0;
		bones[i].currentRotIndex = 0;
		bones[i].currentScaleIndex = 0;
	}
}

Bone::~Bone()
{
	RELEASE_ARRAY(PosKeysValues);
	RELEASE_ARRAY(PosKeysTimes);

	RELEASE_ARRAY(ScaleKeysValues);
	RELEASE_ARRAY(ScaleKeysTimes);

	RELEASE_ARRAY(RotKeysValues);
	RELEASE_ARRAY(RotKeysTimes);
}

bool Bone::calcCurrentIndex(float time)
{
	bool ret = false;

	if (currentPosIndex == -1 || currentRotIndex == -1 || currentScaleIndex == -1)
	{
		currentPosIndex = currentRotIndex = currentScaleIndex = 0;
		return true;
	}

	for (int i = currentPosIndex + 1; i < numPosKeys; i++)
	{
		if (PosKeysTimes[i] <= time && (i + 1 >= numPosKeys || PosKeysTimes[i + 1] > time))
		{
			currentPosIndex = i;
			ret = true;
		}
	}
	for (int i = currentRotIndex + 1; i < numRotKeys; i++)
	{
		if (RotKeysTimes[i] <= time && (i + 1 >= numRotKeys || RotKeysTimes[i + 1] > time))
		{
			currentRotIndex = i;
			ret = true;
		}
	}
	for (int i = currentScaleIndex + 1; i < numScaleKeys; i++)
	{
		if (ScaleKeysTimes[i] <= time && (i + 1 >= numScaleKeys || ScaleKeysTimes[i + 1] > time))
		{
			currentScaleIndex = i;
			ret = true;
		}
	}

	return ret;
}

void Bone::calcTransfrom()
{
	LOG("%d", currentPosIndex);
	vec position = { PosKeysValues[currentPosIndex*3], PosKeysValues[currentPosIndex*3 +1], PosKeysValues[currentPosIndex*3 +2] };
	Quat rotation = { RotKeysValues[currentRotIndex*4], RotKeysValues[currentRotIndex*4 +1], RotKeysValues[currentRotIndex*4 +2], RotKeysValues[currentRotIndex*4 +3] };
	vec scale = { ScaleKeysValues[currentScaleIndex*3], ScaleKeysValues[currentScaleIndex*3 + 1], ScaleKeysValues[currentScaleIndex*3 + 2] };

	lastTransform.Set(float4x4::FromTRS(position, rotation, scale));
}
