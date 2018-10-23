#include "Application.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "GameObject.h"


ComponentMesh::ComponentMesh(GameObject* parent, componentType type) : Component(parent, type)
{
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::DrawInfo()
{
}
