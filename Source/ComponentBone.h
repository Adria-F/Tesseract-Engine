#ifndef __COMPONENTBONE_H__
#define __COMPONENTBONE_H__

#include "Component.h"

class MSphere;
class MLine;

class ComponentBone : public Component
{
public:
	ComponentBone(GameObject* parent, componentType type);
	~ComponentBone();

	bool drawDebugInfo();
	void DrawInfo();

	void Save(JSON_Value* component) const;
	void Load(JSON_Value* component);

public:

	bool debugDraw = false;

	MSphere* sphere = nullptr;
	MLine* line = nullptr;

	float4x4 globalOffset=float4x4::identity;
};

#endif // !__COMPONENTBONE_H__