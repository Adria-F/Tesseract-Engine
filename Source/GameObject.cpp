#include "Application.h"
#include "Globals.h"
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
	UID = GENERATE_UID();
	boundingBox = AABB({ 0,0,0 }, { 0,0,0 });
}


GameObject::~GameObject()
{
	std::list<Component*>::iterator it_c;
	it_c = components.begin();
	while (it_c != components.end())
	{
		RELEASE((*it_c));
		it_c++;
	}
	components.clear();

	int size = childs.size();
	for (int i = 0; i < size; i++)
	{
		RELEASE(childs[i]);
	}
	childs.clear();
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
			{
				ComponentTransformation* transform = (ComponentTransformation*)GetComponent(TRANSFORMATION);

				glPushMatrix();
				glMultMatrixf((float*)transform->globalMatrix.v);
				(*it_c)->Update();
				glPopMatrix();
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		if (App->renderer3D->ShowBB || selected)
		{
			DrawBB(boundingBox, { 0, 0.5f, 1 });
		}

		if (App->renderer3D->ShowBB)
		{
			DrawBB(App->camera->BBtoLook, { 0.8f,0.5f,0.5f });
		}

		for (int i = 0; i < childs.size(); i++)
		{
			childs[i]->Update();
		}		
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

void GameObject::RemoveComponent(Component* component)
{
	components.remove(component);

	RELEASE(component);
}

void GameObject::DrawBB(const AABB& BB, vec3 color) const
{
	glLineWidth(1.5f);
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

void GameObject::Save(JSON_Value* gameobject)
{
	JSON_Value* gameObject = gameobject->createValue();

	gameObject->addUint("UID", UID);
	gameObject->addUint("ParentUID", (parent != nullptr) ? parent->UID : 0);
	gameObject->addString("Name", name.c_str());

	JSON_Value* Components = gameobject->createValue();
	Components->convertToArray();
	for (std::list<Component*>::iterator it_c = components.begin(); it_c != components.end(); it_c++)
	{
		(*it_c)->Save(Components);
	}

	gameObject->addValue("Components", Components);

	gameobject->addValue("", gameObject);

	for (int i = 0; i < childs.size(); i++)
	{
		childs[i]->Save(gameobject);
	}
}

void GameObject::Load(JSON_Value* gameobject)
{
	UID = gameobject->getUint("UID");
	parentUID = gameobject->getUint("ParentUID");
	name = gameobject->getString("Name");

	JSON_Value* Components = gameobject->getValue("Components"); //It is an array of values
	if (Components->getRapidJSONValue()->IsArray()) //Just make sure
	{
		for (int i = 0; i < Components->getRapidJSONValue()->Size(); i++)
		{
			JSON_Value* componentData = Components->getValueFromArray(i); //Get the component data
			Component* component = AddComponent((componentType)componentData->getInt("Type")); //Create the component type
			component->Load(componentData); //Load its info
		}
	}

	for (std::list<Component*>::iterator it_c = components.begin(); it_c != components.end(); it_c++)
	{
		if ((*it_c)->type == MESH)
		{
			boundingBox.SetNegativeInfinity();
			boundingBox.Enclose((float3*)((ComponentMesh*)(*it_c))->mesh->vertices, ((ComponentMesh*)(*it_c))->mesh->num_vertices);
		}
	}
}

Component* GameObject::GetComponent(componentType type)
{
	Component* ret = nullptr;
	for (list<Component*>::iterator it_c = components.begin(); it_c != components.end(); it_c++)
	{
		if ((*it_c)->type == type)
		{
			ret = *it_c;
			break;
		}
	}
	
	return ret;
}
