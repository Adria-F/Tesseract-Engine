#ifndef __COMPONENTTRANSFORM_H__
#define __COMPONENTTRANSFORM_H__

#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class ComponentTransformation : public Component
{
public:

	ComponentTransformation(GameObject* parent, componentType type) :Component(parent, type) {}
	~ComponentTransformation();

	void DrawInfo();
	bool Update();

	void RecalculateMatrix();

	void Save(JSON_Value* component) const;
	void Load(JSON_Value* component);

public:

	float3 position = { 0.0f,0.0f,0.0f };
	float3 rotationVec = { 0.0f,0.0f,0.0f };
	float3 scale = { 1.0f,1.0f,1.0f };

	float4x4 localMatrix = float4x4::identity;
	float4x4 globalMatrix = float4x4::identity;

	Quat rotation = { 1.0f,0.0f,0.0f,0.0f };

	bool changed = false;
};

#endif // !__COMPONENTTRANSFORM_H__