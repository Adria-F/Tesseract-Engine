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

	flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (gameobject->childs.size() <= 0)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 15);
	}


	if (!gameobject->active)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, { 1,0,0,0.5f });
	}

	if (gameobject->selected)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool opened = ImGui::TreeNodeEx(gameobject->name.c_str(), flags);
	if (ImGui::IsItemClicked(0))
	{
		if (App->scene_intro->selected_GO != nullptr)
			App->scene_intro->selected_GO->selected = false;
		App->scene_intro->selected_GO = gameobject;
		gameobject->selected = true;
	}
	if (opened)
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
