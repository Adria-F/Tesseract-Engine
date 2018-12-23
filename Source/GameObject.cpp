#include "Application.h"
#include "Globals.h"
#include "ModuleRenderer3D.h"
#include "ModuleResource.h"
#include "ModuleTextures.h"
#include "ModuleCamera3D.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Component.h"
#include "ComponentTransformation.h"
#include "ComponentMaterial.h"
#include "ComponentAnimation.h"
#include "ComponentMesh.h"
#include "ComponentCamera.h"
#include "ComponentBone.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

GameObject::GameObject()
{
	UID = GENERATE_UID();
	boundingBox = AABB({ 0,0,0 }, { 0,0,0 });
	isStatic = true;
	culling = false;
}


GameObject::~GameObject()
{
	std::list<Component*>::iterator it_cp;
	it_cp = components.begin();
	while (it_cp != components.end())
	{
		RELEASE((*it_cp));
		it_cp++;
	}
	components.clear();

	std::list<GameObject*>::iterator it_c;
	it_c = childs.begin();
	while(it_c != childs.end())
	{
		App->scene_intro->deleteGameObject((*it_c), false);
		it_c++;
	}
	childs.clear();
}

void GameObject::Update(float dt)
{
	if (active)
	{
		if (animation != nullptr)
		{
			if (!App->inGameMode() && animation->TestPlay)
			{
				dt = App->GetDeltaTime();
				
				if (!selected)
				{
					animation->TestPlay = false;
					animation->TestPause = false;
					animation->animTime = 0.0f;
				}
			}
			else
			{
				animation->TestPlay = false;
				animation->TestPause = false;
			}

			animation->Update(dt);
		}

		if (camera != nullptr)
			camera->Update(dt);

		for (std::list<GameObject*>::iterator it_c = childs.begin(); it_c != childs.end(); it_c++)
		{
			(*it_c)->Update(dt);
		}		
	}	

	if (transformation != nullptr && transformation->changed)
	{
		GameObject* bigParent = this;
		while (bigParent->parent != nullptr)
			bigParent = bigParent->parent;
		bigParent->RecalculateBB();

		App->scene_intro->StartQuadTree();
		transformation->changed = false;
	}

	std::list<Component*>::iterator it_c = components.begin();
	while( it_c != components.end())
	{
		if ((*it_c)->toDelete)
		{
			RemoveComponent((*it_c++));
		}
		else
			it_c++;
	}
}

void GameObject::Draw(bool game) const
{
	if (active && (culling || !App->renderer3D->Frustum_Culling))
	{
		App->renderer3D->addToRenderBuffer(this);

		if (!game)
		{
			//Draw the debug info for components
			for (std::list<Component*>::const_iterator it_c = components.begin(); it_c != components.end(); it_c++)
			{
				(*it_c)->drawDebugInfo();
			}

			if (App->renderer3D->ShowBB || selected)
			{
				App->renderer3D->DrawBB(boundingBox, { 0, 0.5f, 1 });
			}
		}

		for (std::list<GameObject*>::const_iterator it_c = childs.begin(); it_c != childs.end(); it_c++)
		{
			(*it_c)->Draw(game);
		}
	}
}

void GameObject::DrawComponentsUI()
{
	for (std::list<Component*>::iterator it_c = components.begin(); it_c != components.end(); it_c++)
	{
		(*it_c)->DrawUI();
		ImGui::Separator();
	}
}

Component* GameObject::AddComponent(componentType type)
{
	Component* ret = nullptr;
	bool itsNew = false;

	switch (type)
	{
	case TRANSFORMATION:
		if (transformation == nullptr)
		{
			transformation = new ComponentTransformation(this, type);
			itsNew = true;
		}
		ret = transformation;
		break;
	case MESH:
		if (mesh == nullptr)
		{
			mesh = new ComponentMesh(this, type);
			itsNew = true;
		}
		ret = mesh;
		break;
	case MATERIAL:
		if (material == nullptr)
		{
			material = new ComponentMaterial(this, type);
			itsNew = true;
		}
		ret = material;
		break;
	case ANIMATION:
		if (animation == nullptr)
		{
			animation = new ComponentAnimation(this, type);
			itsNew = true;
		}
		ret = animation;
		break;
	case CAMERA:
		if (camera == nullptr)
		{
			camera = new ComponentCamera(this, type);
			itsNew = true;
		}
		ret = camera;
		break;
	case BONE:
		ret = new ComponentBone(this, type);
		itsNew = true;
		break;
	}

	if (itsNew)
	{
		components.push_back(ret);
		App->scene_intro->addComponent(ret);
	}

	return ret;
}

void GameObject::RemoveComponent(Component* component)
{
	switch (component->type)
	{
	case TRANSFORMATION:
		transformation = nullptr;
		break;
	case MESH:
		mesh = nullptr;
		if (transformation)
			transformation->changed = true;
		break;
	case MATERIAL:
		material = nullptr;
		break;
	case ANIMATION:
		animation = nullptr;
		break;
	case CAMERA:
		camera = nullptr;
		break;
	}

	App->scene_intro->removeComponent(component);
	components.remove(component);
	RELEASE(component);
}

void GameObject::Save(JSON_Value* gameobject)
{
	JSON_Value* gameObject = gameobject->createValue();

	gameObject->addUint("UID", UID);
	gameObject->addUint("ParentUID", (parent == App->scene_intro->root || parent == nullptr) ? 0 : parent->UID);
	gameObject->addString("Name", name.c_str());
	gameObject->addBool("Active", active);
	gameObject->addBool("Static", isStatic);

	JSON_Value* Components = gameobject->createValue();
	Components->convertToArray();

	for (std::list<Component*>::iterator it_c = components.begin(); it_c != components.end(); it_c++)
	{
		(*it_c)->Save(Components);
	}

	gameObject->addValue("Components", Components);

	gameobject->addValue("", gameObject);

	for (std::list<GameObject*>::iterator it_c = childs.begin(); it_c != childs.end(); it_c++)
	{
		(*it_c)->Save(gameobject);
	}
}

uint GameObject::Load(JSON_Value* gameobject, std::map<uint, uint>& LinkerComponents)
{
	parentUID = gameobject->getUint("ParentUID");
	name = gameobject->getString("Name");
	active= gameobject->getBool("Active");
	isStatic = gameobject->getBool("Static");

	JSON_Value* Components = gameobject->getValue("Components"); //It is an array of values

	if (Components->getRapidJSONValue()->IsArray()) //Just make sure
	{
		for (int i = 0; i < Components->getRapidJSONValue()->Size(); i++)
		{
			JSON_Value* componentData = Components->getValueFromArray(i); //Get the component data
			Component* component = AddComponent((componentType)componentData->getInt("Type")); //Create the component type
			component->Load(componentData); //Load its info

			LinkerComponents[componentData->getUint("UID")] = component->UID;
		}
	}

	return gameobject->getUint("UID");
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

void GameObject::RecalculateBB()
{
	if (transformation != nullptr)
	{
		boundingBox.SetNegativeInfinity();

		if (childs.size() > 0)
		{
			for (std::list<GameObject*>::iterator it_c = childs.begin(); it_c != childs.end(); it_c++)
			{
				(*it_c)->RecalculateBB();
				if ((*it_c)->boundingBox.IsFinite())
					boundingBox.Enclose((*it_c)->boundingBox);
			}
		}

		if (mesh!=nullptr)
		{
			ResourceMesh* rMesh = (ResourceMesh*)App->resources->GetResource(mesh->RUID);
			if (rMesh != nullptr)
				boundingBox.Enclose((float3*)rMesh->vertices, rMesh->num_vertices);
		}

		if (childs.size() <= 0)
		{
			boundingBox.TransformAsAABB(transformation->globalMatrix);
			if(mesh == nullptr)
				boundingBox = AABB({ 0,0,0 }, { 0,0,0 });
		}
	}
}

void GameObject::setSelected(bool selected)
{
	this->selected = selected;

	if (parent != nullptr)
		parent->setChildSelected(selected);
}

void GameObject::setChildSelected(bool selected)
{
	child_selected = selected;

	if (parent != nullptr)
		parent->setChildSelected(selected);
}

void GameObject::changeParent(GameObject* newParent, bool recalculateTransformation)
{
	if (newParent != parent && newParent != nullptr)
	{
		bool isChild = false;
		GameObject* curr = newParent;
		while (curr->parent != nullptr)
		{
			if (curr->parent == this)
				return; //If we are trying to insert a parent object inside one of its childs, ignore it
			curr = curr->parent;
		}

		if (parent != nullptr)
		{
			parent->childs.remove(this);
			parent->child_selected = false;
		}

		parent = newParent;
		parent->childs.push_back(this);
		parent->child_selected = true;

		if (recalculateTransformation)
		{
			//Update the transformation
			ComponentTransformation* transform = (ComponentTransformation*)GetComponent(TRANSFORMATION);
			transform->localMatrix = ((ComponentTransformation*)parent->GetComponent(TRANSFORMATION))->globalMatrix.Inverted()*transform->globalMatrix.Transposed();
			transform->localMatrix.Decompose(transform->position, transform->rotation, transform->scale);
			transform->changed = true;
		}
	}
}

GameObject* GameObject::getChildByName(const char* name) const
{
	GameObject* child = nullptr;

	for (std::list<GameObject*>::const_iterator it_go = childs.begin(); it_go != childs.end(); it_go++)
	{
		if ((*it_go)->name == name)
		{
			child = (*it_go);
			break;
		}
		else
		{
			child = (*it_go)->getChildByName(name);
			if (child != nullptr)
				break;
		}
	}

	return child;
}
