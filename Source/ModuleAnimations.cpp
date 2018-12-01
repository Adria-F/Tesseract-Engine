#include "ModuleAnimations.h"
#include "Application.h"
#include "Globals.h"
#include "ModuleFileSystem.h"
#include "Resource.h"
#include "ResourceAnimation.h"

#include "Assimp/include/anim.h"

//#ifdef _DEBUG
////#define TEST_MEMORY_MANAGER
//#include "mmgr/mmgr.h"
//#endif

ModuleAnimations::ModuleAnimations(bool start_enabled) : Module(start_enabled)
{
}

ModuleAnimations::~ModuleAnimations()
{
}

bool ModuleAnimations::importAnimation(aiAnimation* animation, uint UID, std::string & newpath)
{
	bool ret = false;

	ResourceAnimation* newAnimation = new ResourceAnimation(0, R_ANIMATION);

	newAnimation->time = animation->mDuration;
	newAnimation->ticksXsecond = animation->mTicksPerSecond;
	newAnimation->numBones = animation->mNumChannels;

	if (animation->mNumChannels > 0)
	{
		newAnimation->bones = new Bone[newAnimation->numBones];
		for (int i = 0; i < newAnimation->numBones; i++)
		{
			newAnimation->bones[i].NodeName = (animation->mChannels[i]->mNodeName.length > 0) ? animation->mChannels[i]->mNodeName.C_Str() : "Unnamed";

			newAnimation->bones[i].numPosKeys = animation->mChannels[i]->mNumPositionKeys;
			newAnimation->bones[i].PosKeysValues = new float[newAnimation->bones[i].numPosKeys * 3];
			newAnimation->bones[i].PosKeysTimes = new double[newAnimation->bones[i].numPosKeys];
			for (int j = 0; j < newAnimation->bones[i].numPosKeys; j++)
			{
				int Jvalue = j * 3;
				newAnimation->bones[i].PosKeysTimes[j] = animation->mChannels[i]->mPositionKeys[j].mTime;
				newAnimation->bones[i].PosKeysValues[Jvalue] = animation->mChannels[i]->mPositionKeys[j].mValue.x;
				newAnimation->bones[i].PosKeysValues[Jvalue + 1] = animation->mChannels[i]->mPositionKeys[j].mValue.y;
				newAnimation->bones[i].PosKeysValues[Jvalue + 2] = animation->mChannels[i]->mPositionKeys[j].mValue.z;
			}

			newAnimation->bones[i].numScaleKeys = animation->mChannels[i]->mNumScalingKeys;
			newAnimation->bones[i].ScaleKeysValues = new float[newAnimation->bones[i].numScaleKeys * 3];
			newAnimation->bones[i].ScaleKeysTimes = new double[newAnimation->bones[i].numScaleKeys];
			for (int j = 0; j < newAnimation->bones[i].numScaleKeys; j++)
			{
				int Jvalue = j * 3;
				newAnimation->bones[i].ScaleKeysTimes[j] = animation->mChannels[i]->mScalingKeys[j].mTime;
				newAnimation->bones[i].ScaleKeysValues[Jvalue] = animation->mChannels[i]->mScalingKeys[j].mValue.x;
				newAnimation->bones[i].ScaleKeysValues[Jvalue + 1] = animation->mChannels[i]->mScalingKeys[j].mValue.y;
				newAnimation->bones[i].ScaleKeysValues[Jvalue + 2] = animation->mChannels[i]->mScalingKeys[j].mValue.z;
			}

			newAnimation->bones[i].numRotKeys = animation->mChannels[i]->mNumRotationKeys;
			newAnimation->bones[i].RotKeysValues = new float[newAnimation->bones[i].numRotKeys * 4];
			newAnimation->bones[i].RotKeysTimes = new double[newAnimation->bones[i].numRotKeys];
			for (int j = 0; j < newAnimation->bones[i].numRotKeys; j++)
			{
				int Jvalue = j * 4;
				newAnimation->bones[i].RotKeysTimes[j] = animation->mChannels[i]->mRotationKeys[j].mTime;
				newAnimation->bones[i].RotKeysValues[Jvalue] = animation->mChannels[i]->mRotationKeys[j].mValue.x;
				newAnimation->bones[i].RotKeysValues[Jvalue + 1] = animation->mChannels[i]->mRotationKeys[j].mValue.y;
				newAnimation->bones[i].RotKeysValues[Jvalue + 2] = animation->mChannels[i]->mRotationKeys[j].mValue.z;
				newAnimation->bones[i].RotKeysValues[Jvalue + 3] = animation->mChannels[i]->mRotationKeys[j].mValue.w;
			}
		}

		ret = true;
	}

	if (ret)
		saveAnimation(newAnimation, UID, newpath);

	RELEASE(newAnimation);

	return ret;
}

bool ModuleAnimations::saveAnimation(ResourceAnimation* animation, uint UID, std::string & newpath)
{
	bool ret = false;

	uint ranges[3] = { animation->time, animation->ticksXsecond, animation->numBones };
	uint size = sizeof(ranges);
	for (int i = 0; i < animation->numBones; i++)
	{
		uint boneRanges[3] = { animation->bones[i].numPosKeys, animation->bones[i].numScaleKeys, animation->bones[i].numRotKeys };
		size += sizeof(boneRanges);
		size += animation->bones[i].NodeName.size();

		size += sizeof(double)*animation->bones[i].numPosKeys;
		size += sizeof(float)*animation->bones[i].numPosKeys*3;

		size += sizeof(double)*animation->bones[i].numScaleKeys;
		size += sizeof(float)*animation->bones[i].numScaleKeys * 3;

		size += sizeof(double)*animation->bones[i].numRotKeys;
		size += sizeof(float)*animation->bones[i].numRotKeys * 4;
	}

	char* buffer = new char[size];
	char* cursor = buffer;

	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	for (int i = 0; i < animation->numBones; i++)
	{
		uint boneRanges[3] = { animation->bones[i].numPosKeys, animation->bones[i].numScaleKeys, animation->bones[i].numRotKeys };

		bytes = sizeof(boneRanges);
		memcpy(cursor, boneRanges, bytes);
		cursor += bytes;

		bytes = animation->bones[i].NodeName.size();
		memcpy(cursor, animation->bones[i].NodeName.c_str(), bytes);
		cursor += bytes;

		bytes = animation->bones[i].numPosKeys * sizeof(double);
		memcpy(cursor, animation->bones[i].PosKeysTimes, bytes);
		cursor += bytes;
		bytes = animation->bones[i].numPosKeys * sizeof(float)*3;
		memcpy(cursor, animation->bones[i].PosKeysValues, bytes);
		cursor += bytes;

		bytes = animation->bones[i].numScaleKeys * sizeof(double);
		memcpy(cursor, animation->bones[i].ScaleKeysTimes, bytes);
		cursor += bytes;
		bytes = animation->bones[i].numScaleKeys * sizeof(float) * 3;
		memcpy(cursor, animation->bones[i].ScaleKeysValues, bytes);
		cursor += bytes;

		bytes = animation->bones[i].numRotKeys * sizeof(double);
		memcpy(cursor, animation->bones[i].RotKeysTimes, bytes);
		cursor += bytes;
		bytes = animation->bones[i].numRotKeys * sizeof(float) * 4;
		memcpy(cursor, animation->bones[i].RotKeysValues, bytes);
		cursor += bytes;
	}

	App->fileSystem->writeFile((ANIMATIONS_FOLDER + std::to_string(UID) + ANIMATION_EXTENSION).c_str(), buffer, size, true);
	RELEASE_ARRAY(buffer);

	newpath = ANIMATIONS_FOLDER + std::to_string(UID) + ANIMATION_EXTENSION;

	return ret;
}
