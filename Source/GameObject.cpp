#include "Application.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentTransformation.h"
#include "ComponentTexture.h"
#include "ComponentMesh.h"


GameObject::GameObject()
{
}


GameObject::~GameObject()
{
}

void GameObject::Update()
{

}

Component* GameObject::AddComponent(componentType type)
{
	Component* ret;

	switch (type)
	{
	case TRANSFORMATION:
		ret = new ComponentTransformation(this,type);
		components.push_back(ret);
		break;
	case MESH:
		ret=new ComponentMesh(this, type);
		components.push_back(ret);
		break;
	case MATERIAL:
		ret = new ComponentTexture(this, type);
		components.push_back(ret);
		break;
	}

	return ret;
}

void GameObject::RemoveComponent(Component& component)
{

}

ComponentTransformation* GameObject::CreateComponentTransformation()
{
	ComponentTransformation* ret;

	return ret;
}