#ifndef __COMPONENTCAMERA_H__
#define __COMPONENTCAMERA_H__

#include "Component.h"

class ComponentCamera :	public Component
{
public:

	ComponentCamera(GameObject* gameobject, componentType type);
	~ComponentCamera();

	float* getViewMatrix();
	float* getProjectionMatrix();

	void setAspectRatio(float aspectRatio);
	void DrawFrustum();

	void DrawInfo();

	void Save(JSON_Value* component) const;
	void Load(JSON_Value* component);

public:

	Frustum frustum;

};

#endif
