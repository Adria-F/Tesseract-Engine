#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class ComponentTransformation : public Component
{
public:
	ComponentTransformation();
	~ComponentTransformation();

	void DrawInfo();
	bool Update();

public:

	float3 position = { 0.0f,0.0f,0.0f };

	Quat rotation = { 1.0f,0.0f,0.0f,0.0f };
	float3 rotationVec = { 0.0f,0.0f,0.0f };
	
	float3 scale = { 1.0f,1.0f,1.0f };
};

#endif // !__COMPONENTTRANSFORM_H__