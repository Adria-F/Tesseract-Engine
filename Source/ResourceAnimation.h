#ifndef __RESOURCEANIMATION_H__
#define __RESOURCEANIMATION_H__

#include "Resource.h"

struct PositionKey
{
	double time;
	vec Value;
};

struct ScalingKey
{
	double time;
	vec Value;
};

struct RotationKey
{
	double time;
	Quat Value;
};

struct Bone
{
	std::string NodeName = "";
	
	int numPosKeys = 0;
	int numScalKeys = 0;
	int numRotKeys = 0;	


	PositionKey* PosKeys = nullptr;
	ScalingKey* ScalKeys  = nullptr;
	RotationKey* RotKeys = nullptr;
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


