#ifndef __RESOURCEANIMATION_H__
#define __RESOURCEANIMATION_H__

#include "Resource.h"

struct Bone
{
	~Bone();

	std::string NodeName = "";
	
	int numPosKeys = 0;
	int numScaleKeys = 0;
	int numRotKeys = 0;	

	float* PosKeysValues = nullptr;
	double* PosKeysTimes = nullptr;

	float* ScaleKeysValues = nullptr;
	double* ScaleKeysTimes = nullptr;

	float* RotKeysValues = nullptr;
	double* RotKeysTimes = nullptr;
};

class ResourceAnimation : public Resource
{
public:

	ResourceAnimation(uint UID, ResType type);
	~ResourceAnimation();

	static void setImportDefaults(JSON_Value& importSettings);

	bool LoadInMemory();
	bool LoadAnimation();
	bool UnloadFromMemory();

public:

	std::string name = "";
	
	float time = 0.0f;
	float ticksXsecond = 0.0f;
	
	int numBones=0;

	Bone* bones=nullptr;

};

#endif // !__RESOURCEANIMATION_H__


