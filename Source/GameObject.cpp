#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
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
	if (active)
	{
		for (std::list<Component*>::iterator it_c = components.begin(); it_c != components.end(); it_c++)
		{
			if ((*it_c)->type == MATERIAL)
				(*it_c)->Update();
		}

		for (std::list<Component*>::iterator it_c = components.begin(); it_c != components.end(); it_c++)
		{
			if ((*it_c)->type == MESH)
				(*it_c)->Update();
		}
	}
	

	glBindTexture(GL_TEXTURE_2D, 0);
}

void GameObject::Draw()
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