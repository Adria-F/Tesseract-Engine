#ifndef __COMPONENTTEXTURE_H__
#define __COMPONENTTEXTURE_H__

#include "Component.h"

class Texture;

class ComponentTexture : public Component
{
public:
	
	ComponentTexture(GameObject* gameobject, componentType type) :Component(gameobject, type) {}

	~ComponentTexture();

	bool Update();

	void DrawInfo();

public: 

	Texture* Material;

};

#endif // !__COMPONENTTEXTURE_H__
