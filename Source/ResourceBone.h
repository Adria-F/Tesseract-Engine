#ifndef __RESOURCEBONE_H__
#define __RESOURCEBONE_H__

#include "Resource.h"

struct Weight {

	uint VertexID;
	float weight;

};

class ResourceBone : public Resource
{
public:
	ResourceBone(uint UID, ResType type);
	~ResourceBone();

	bool LoadInMemory();
	bool LoadBone();
	bool UnloadFromMemory();

public:

	
	uint numWeights;

	float4x4 Offset;
	float* pos;
	float* rot;
	float* scale;

	Weight* weights;


};

#endif // !__RESOURCEBONE_H__