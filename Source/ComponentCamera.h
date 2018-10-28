#ifndef __COMPONENTCAMERA_H__
#define __COMPONENTCAMERA_H__

#include "Component.h"

class ComponentCamera :	public Component
{
public:

	ComponentCamera(GameObject* gameobject, componentType type);
	~ComponentCamera();

	float* getViewMatrix();
	float* getViewProjMatrix();

public:

	Frustum* frustum= nullptr;

};

#endif
