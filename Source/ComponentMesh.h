#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Component.h"

class Mesh;
class GameObject;
class ResourceMesh;

class ComponentMesh : public Component
{
public:
	
	ComponentMesh(GameObject* parent, componentType type);

	~ComponentMesh();

	bool Update(float dt);
	void DrawInfo();

	void Save(JSON_Value* component) const;
	void Load(JSON_Value* component);

private:

	bool color=true;

};

#endif // !__COMPONENTMESH_H__
