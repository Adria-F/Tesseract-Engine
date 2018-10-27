#include "Application.h"
#include "Panel.h"
#include "PanelHierarchy.h"
#include "ModuleScene.h"
#include "GameObject.h"



PanelHierarchy::PanelHierarchy(const char* name): Panel(name)
{
	active = true;
}

PanelHierarchy::~PanelHierarchy()
{
}

void PanelHierarchy::Draw()
{
	ImGui::Begin("Game Objects");

	if (App->scene_intro->GameObjects.size() > 0)
	{
		for (int i = 0; i < App->scene_intro->GameObjects.size(); i++)
		{
			GameObject* gameobject = App->scene_intro->GameObjects[i];
			if (gameobject->parent == nullptr)
			{
				FillTree(gameobject);
			}
		}
	}

	ImGui::End();
}

void PanelHierarchy::FillTree(GameObject * gameobject)
{
	uint flags;

	flags = ImGuiTreeNodeFlags_OpenOnArrow;

	if (gameobject->childs.size() <= 0)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
		flags |= ImGuiTreeNodeFlags_Bullet;
	}


	if (!gameobject->active)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { 1,0,0,1 });
	}
	
	if (ImGui::TreeNodeEx(gameobject->name.c_str(), flags))
	{
		if (gameobject->childs.size() > 0)
		{
			for (int i = 0; i < gameobject->childs.size(); i++)
			{
				FillTree(gameobject->childs[i]);
			}
		}
		ImGui::TreePop();
	}

	if (!gameobject->active)
	{
		ImGui::PopStyleColor();
	}
}
