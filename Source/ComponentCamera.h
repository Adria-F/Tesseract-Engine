#ifndef __COMPONENTCAMERA_H__
#define __COMPONENTCAMERA_H__

#include "Component.h"

class ComponentCamera :	public Component
{
public:

	ComponentCamera(GameObject* gameobject, componentType type);
	~ComponentCamera();

	bool Update();

	float* getViewMatrix();
	float* getProjectionMatrix();

	void setAspectRatio(float aspectRatio);
	void DrawFrustum();

	void DrawInfo();

	bool ContainsAABB(const AABB& boundingBox);

	void Save(JSON_Value* component) const;
	void Load(JSON_Value* component);

	void CameraBB();

public:

	Frustum frustum;
	AABB cameraBB;
	bool IsCulling =false;
	float4x4 currentTransform;
};

#endif
