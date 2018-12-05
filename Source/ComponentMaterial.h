#ifndef __COMPONENTMATERIAL_H__
#define __COMPONENTMATERIAL_H__

#include "Component.h"

class Texture;
class ResourceMaterial;

enum MaterialType
{
	COLOR = 0,
	TEXTURE
};

class ComponentMaterial : public Component
{
public:
	
	ComponentMaterial(GameObject* gameobject, componentType type);

	~ComponentMaterial();

	bool Update(float dt);
	void DrawInfo();

	void Save(JSON_Value* component) const;
	void Load(JSON_Value* component);

public:

	float alphaTest = 0.0f;

	MaterialType materialType = COLOR;

	bool transparentColor = false;
	bool doAlphaTest = false;
	bool doBlendColors = false;
};

#endif // !__COMPONENTMATERIAL_H__
