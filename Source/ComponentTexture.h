#ifndef __COMPONENTTEXTURE_H__
#define __COMPONENTTEXTURE_H__

#include "Component.h"

class ComponentTexture : public Component
{
public:
	
	ComponentTexture(GameObject* parent, componentType type) :Component(parent, type) {}

	~ComponentTexture();

	void DrawInfo();

};

#endif // !__COMPONENTTEXTURE_H__
