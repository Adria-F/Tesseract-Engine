#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Component.h"

class Mesh;
class ComponentMesh : public Component
{
public:
	
	ComponentMesh(GameObject* parent, componentType type) :Component(parent, type) {}

	~ComponentMesh();

	void DrawInfo();

public:

	Mesh* mesh;

	//bool wire;

};

#endif // !__COMPONENTMESH_H__
