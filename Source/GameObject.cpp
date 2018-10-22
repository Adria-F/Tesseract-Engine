#include "Application.h"
#include "ModuleRenderer3D.h"
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
		ret=CreateComponentTransformation();
		break;
	case MESH:
		ret=App->renderer3D->CreateComponentMesh();
		break;
	case MATERIAL:
		ret = App->renderer3D->CreateComponentTexture();
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