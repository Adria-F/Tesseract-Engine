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

	newAnimation->ticks = animation->mDuration;
	newAnimation->ticksXsecond = animation->mTicksPerSecond;
	newAnimation->numBones = animation->mNumChannels;

	if (animation->mNumChannels > 0)
	{
		newAnimation->boneTransformations = new BoneTransform[newAnimation->numBones];
		for (int i = 0; i < newAnimation->numBones; i++)
		{
			//TODO manage dummy nodes
			newAnimation->boneTransformations[i].NodeName = (animation->mChannels[i]->mNodeName.length > 0) ? animation->mChannels[i]->mNodeName.C_Str() : "Unnamed";

			newAnimation->boneTransformations[i].numPosKeys = animation->mChannels[i]->mNumPositionKeys;
			newAnimation->boneTransformations[i].PosKeysValues = new float[newAnimation->boneTransformations[i].numPosKeys * 3];
			newAnimation->boneTransformations[i].PosKeysTimes = new double[newAnimation->boneTransformations[i].numPosKeys];
			for (int j = 0; j < newAnimation->boneTransformations[i].numPosKeys; j++)
			{
				int Jvalue = j * 3;
				newAnimation->boneTransformations[i].PosKeysTimes[j] = animation->mChannels[i]->mPositionKeys[j].mTime;
				newAnimation->boneTransformations[i].PosKeysValues[Jvalue] = animation->mChannels[i]->mPositionKeys[j].mValue.x;
				newAnimation->boneTransformations[i].PosKeysValues[Jvalue + 1] = animation->mChannels[i]->mPositionKeys[j].mValue.y;
				newAnimation->boneTransformations[i].PosKeysValues[Jvalue + 2] = animation->mChannels[i]->mPositionKeys[j].mValue.z;
			}

			newAnimation->boneTransformations[i].numScaleKeys = animation->mChannels[i]->mNumScalingKeys;
			newAnimation->boneTransformations[i].ScaleKeysValues = new float[newAnimation->boneTransformations[i].numScaleKeys * 3];
			newAnimation->boneTransformations[i].ScaleKeysTimes = new double[newAnimation->boneTransformations[i].numScaleKeys];
			for (int j = 0; j < newAnimation->boneTransformations[i].numScaleKeys; j++)
			{
				int Jvalue = j * 3;
				newAnimation->boneTransformations[i].ScaleKeysTimes[j] = animation->mChannels[i]->mScalingKeys[j].mTime;
				newAnimation->boneTransformations[i].ScaleKeysValues[Jvalue] = animation->mChannels[i]->mScalingKeys[j].mValue.x;
				newAnimation->boneTransformations[i].ScaleKeysValues[Jvalue + 1] = animation->mChannels[i]->mScalingKeys[j].mValue.y;
				newAnimation->boneTransformations[i].ScaleKeysValues[Jvalue + 2] = animation->mChannels[i]->mScalingKeys[j].mValue.z;
			}

			newAnimation->boneTransformations[i].numRotKeys = animation->mChannels[i]->mNumRotationKeys;
			newAnimation->boneTransformations[i].RotKeysValues = new float[newAnimation->boneTransformations[i].numRotKeys * 4];
			newAnimation->boneTransformations[i].RotKeysTimes = new double[newAnimation->boneTransformations[i].numRotKeys];
			for (int j = 0; j < newAnimation->boneTransformations[i].numRotKeys; j++)
			{
				int Jvalue = j * 4;
				newAnimation->boneTransformations[i].RotKeysTimes[j] = animation->mChannels[i]->mRotationKeys[j].mTime;
				newAnimation->boneTransformations[i].RotKeysValues[Jvalue] = animation->mChannels[i]->mRotationKeys[j].mValue.x;
				newAnimation->boneTransformations[i].RotKeysValues[Jvalue + 1] = animation->mChannels[i]->mRotationKeys[j].mValue.y;
				newAnimation->boneTransformations[i].RotKeysValues[Jvalue + 2] = animation->mChannels[i]->mRotationKeys[j].mValue.z;
				newAnimation->boneTransformations[i].RotKeysValues[Jvalue + 3] = animation->mChannels[i]->mRotationKeys[j].mValue.w;
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

	uint ranges[3] = { animation->ticks, animation->ticksXsecond, animation->numBones };
	uint size = sizeof(ranges);
	for (int i = 0; i < animation->numBones; i++)
	{
		uint boneRanges[4] = { animation->boneTransformations[i].numPosKeys, animation->boneTransformations[i].numScaleKeys, animation->boneTransformations[i].numRotKeys, animation->boneTransformations[i].NodeName.size()};
		size += sizeof(boneRanges);
		size += animation->boneTransformations[i].NodeName.size();

		size += sizeof(double)*animation->boneTransformations[i].numPosKeys;
		size += sizeof(float)*animation->boneTransformations[i].numPosKeys*3;

		size += sizeof(double)*animation->boneTransformations[i].numScaleKeys;
		size += sizeof(float)*animation->boneTransformations[i].numScaleKeys * 3;

		size += sizeof(double)*animation->boneTransformations[i].numRotKeys;
		size += sizeof(float)*animation->boneTransformations[i].numRotKeys * 4;
	}

	char* buffer = new char[size];
	char* cursor = buffer;

	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	for (int i = 0; i < animation->numBones; i++)
	{
		uint boneRanges[4] = { animation->boneTransformations[i].numPosKeys, animation->boneTransformations[i].numScaleKeys, animation->boneTransformations[i].numRotKeys, animation->boneTransformations[i].NodeName.size() };

		bytes = sizeof(boneRanges);
		memcpy(cursor, boneRanges, bytes);
		cursor += bytes;

		bytes = animation->boneTransformations[i].NodeName.size();
		memcpy(cursor, animation->boneTransformations[i].NodeName.c_str(), bytes);
		cursor += bytes;

		bytes = animation->boneTransformations[i].numPosKeys * sizeof(double);
		memcpy(cursor, animation->boneTransformations[i].PosKeysTimes, bytes);
		cursor += bytes;
		bytes = animation->boneTransformations[i].numPosKeys * sizeof(float)*3;
		memcpy(cursor, animation->boneTransformations[i].PosKeysValues, bytes);
		cursor += bytes;

		bytes = animation->boneTransformations[i].numScaleKeys * sizeof(double);
		memcpy(cursor, animation->boneTransformations[i].ScaleKeysTimes, bytes);
		cursor += bytes;
		bytes = animation->boneTransformations[i].numScaleKeys * sizeof(float) * 3;
		memcpy(cursor, animation->boneTransformations[i].ScaleKeysValues, bytes);
		cursor += bytes;

		bytes = animation->boneTransformations[i].numRotKeys * sizeof(double);
		memcpy(cursor, animation->boneTransformations[i].RotKeysTimes, bytes);
		cursor += bytes;
		bytes = animation->boneTransformations[i].numRotKeys * sizeof(float) * 4;
		memcpy(cursor, animation->boneTransformations[i].RotKeysValues, bytes);
		cursor += bytes;
	}

	App->fileSystem->writeFile((ANIMATIONS_FOLDER + std::to_string(UID) + ANIMATION_EXTENSION).c_str(), buffer, size, true);
	RELEASE_ARRAY(buffer);

	newpath = ANIMATIONS_FOLDER + std::to_string(UID) + ANIMATION_EXTENSION;

	return ret;
}
