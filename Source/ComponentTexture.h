#ifndef __COMPONENTTEXTURE_H__
#define __COMPONENTTEXTURE_H__

#include "Component.h"

class Texture;
class ResourceTexture;

class ComponentTexture : public Component
{
public:
	
	ComponentTexture(GameObject* gameobject, componentType type) :Component(gameobject, type) { Material = nullptr; }

	~ComponentTexture();

	bool Update();
	void DrawInfo();

	void Save(JSON_Value* component) const;
	void Load(JSON_Value* component);

public: 

	Texture* Material = nullptr;
	ResourceTexture* resource = nullptr;

};

#endif // !__COMPONENTTEXTURE_H__
