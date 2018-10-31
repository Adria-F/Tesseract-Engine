#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"
#include "ModuleCamera3D.h"
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

		if (App->renderer3D->ShowBB)
		{
			DrawBB(boundingBox, { 0, 0.5f, 1 });
		}

		if (App->renderer3D->ShowBB && App->camera->BBtoLook != nullptr)
		{
			DrawBB(*App->camera->BBtoLook, { 0.8f,0.5f,0.5f });
		}

		for (int i = 0; i < childs.size(); i++)
		{
			childs[i]->Update();
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}	
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

void GameObject::DrawBB(const AABB& BB, vec3 color) const
{
	glLineWidth(2.0f);
	glColor3f(color.x, color.y, color.z);

	glBegin(GL_LINES);

	glVertex3f(BB.CornerPoint(0).x, BB.CornerPoint(0).y, BB.CornerPoint(0).z);
	glVertex3f(BB.CornerPoint(1).x, BB.CornerPoint(1).y, BB.CornerPoint(1).z);

	glVertex3f(BB.CornerPoint(0).x, BB.CornerPoint(0).y, BB.CornerPoint(0).z);
	glVertex3f(BB.CornerPoint(2).x, BB.CornerPoint(2).y, BB.CornerPoint(2).z);

	glVertex3f(BB.CornerPoint(0).x, BB.CornerPoint(0).y, BB.CornerPoint(0).z);
	glVertex3f(BB.CornerPoint(4).x, BB.CornerPoint(4).y, BB.CornerPoint(4).z);

	glVertex3f(BB.CornerPoint(3).x, BB.CornerPoint(3).y, BB.CornerPoint(3).z);
	glVertex3f(BB.CornerPoint(1).x, BB.CornerPoint(1).y, BB.CornerPoint(1).z);

	glVertex3f(BB.CornerPoint(3).x, BB.CornerPoint(3).y, BB.CornerPoint(3).z);
	glVertex3f(BB.CornerPoint(2).x, BB.CornerPoint(2).y, BB.CornerPoint(2).z);

	glVertex3f(BB.CornerPoint(3).x, BB.CornerPoint(3).y, BB.CornerPoint(3).z);
	glVertex3f(BB.CornerPoint(7).x, BB.CornerPoint(7).y, BB.CornerPoint(7).z);

	glVertex3f(BB.CornerPoint(6).x, BB.CornerPoint(6).y, BB.CornerPoint(6).z);
	glVertex3f(BB.CornerPoint(2).x, BB.CornerPoint(2).y, BB.CornerPoint(2).z);

	glVertex3f(BB.CornerPoint(6).x, BB.CornerPoint(6).y, BB.CornerPoint(6).z);
	glVertex3f(BB.CornerPoint(4).x, BB.CornerPoint(4).y, BB.CornerPoint(4).z);

	glVertex3f(BB.CornerPoint(6).x, BB.CornerPoint(6).y, BB.CornerPoint(6).z);
	glVertex3f(BB.CornerPoint(7).x, BB.CornerPoint(7).y, BB.CornerPoint(7).z);

	glVertex3f(BB.CornerPoint(5).x, BB.CornerPoint(5).y, BB.CornerPoint(5).z);
	glVertex3f(BB.CornerPoint(1).x, BB.CornerPoint(1).y, BB.CornerPoint(1).z);

	glVertex3f(BB.CornerPoint(5).x, BB.CornerPoint(5).y, BB.CornerPoint(5).z);
	glVertex3f(BB.CornerPoint(4).x, BB.CornerPoint(4).y, BB.CornerPoint(4).z);

	glVertex3f(BB.CornerPoint(5).x, BB.CornerPoint(5).y, BB.CornerPoint(5).z);
	glVertex3f(BB.CornerPoint(7).x, BB.CornerPoint(7).y, BB.CornerPoint(7).z);

	glEnd();

	glColor3f(1, 1, 1);
	glLineWidth(1.0f);
}
