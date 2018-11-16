#include "Application.h"
#include "Panel.h"
#include "PanelHierarchy.h"
#include "ModuleScene.h"
#include "Component.h"
#include "ComponentTransformation.h"
#include "ComponentCamera.h"
#include "GameObject.h"

PanelHierarchy::PanelHierarchy(const char* name): Panel(name)
{
	active = true;
	creating = true;
}

PanelHierarchy::~PanelHierarchy()
{
}

void PanelHierarchy::Draw()
{
	ImGui::Begin("Game Objects",&active, ImGuiWindowFlags_MenuBar);

	ImGui::BeginMenuBar();
	ImGui::PushStyleColor(ImGuiCol_Button, { 0.9f,0.45f,0.0f,0.7f });

	ImGui::Button("Create");
	
	if (creating)
	{
		if (ImGui::BeginPopupContextItem("create", 0))
		{
			if (ImGui::Button("Camera"))
			{
				GameObject* newGameObject = new GameObject();
				App->scene_intro->addGameObject(newGameObject, App->scene_intro->root);
				newGameObject->name = "Camera";

				ComponentTransformation* GOTransform = (ComponentTransformation*)newGameObject->AddComponent(TRANSFORMATION);

				float3 pos = float3::zero;
				float3 scale = float3::one;
				Quat rot = Quat::identity;

				GOTransform->position = pos;
				GOTransform->scale = scale;
				GOTransform->rotation = rot;
				GOTransform->localMatrix.Set(float4x4::FromTRS(pos, rot, scale));

				newGameObject->camera = (ComponentCamera*)newGameObject->AddComponent(CAMERA);

				newGameObject->boundingBox = newGameObject->camera->cameraBB;

				creating = false;
			}

			if (ImGui::Button("Empty Object"))
			{
				GameObject* newGameObject = new GameObject();
				App->scene_intro->addGameObject(newGameObject, App->scene_intro->root);
				newGameObject->name = "Empty Object";

				ComponentTransformation* GOTransform = (ComponentTransformation*)newGameObject->AddComponent(TRANSFORMATION);

				float3 pos = float3::zero;
				float3 scale = float3::one;
				Quat rot = Quat::identity;

				GOTransform->position = pos;
				GOTransform->scale = scale;
				GOTransform->rotation = rot;
				GOTransform->localMatrix.Set(float4x4::FromTRS(pos, rot, scale));

				creating = false;
			}
			ImGui::EndPopup();
		}
	}
	else
		creating = true;

	ImGui::PopStyleColor();
	ImGui::EndMenuBar();

	for (std::list<GameObject*>::iterator it_ch = App->scene_intro->root->childs.begin(); it_ch != App->scene_intro->root->childs.end(); it_ch++)
	{
		FillTree((*it_ch));
	}

	ImGui::End();

	if (creating)
	{
		App->scene_intro->StartQuadTree();
	}
}

void PanelHierarchy::FillTree(GameObject* gameobject)
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

	if (gameobject->child_selected)
	{
		ImGui::SetNextTreeNodeOpen(true);
	}

	bool opened = ImGui::TreeNodeEx(gameobject->name.c_str(), flags);
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("GAME_OBJECT", &gameobject->UID, sizeof(uint));
		ImGui::Text(gameobject->name.c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAME_OBJECT", ImGuiDragDropFlags_SourceAllowNullID))
		{
			GameObject* draggedGameobject = App->scene_intro->getGameObject(*(uint*)payload->Data);
			if (draggedGameobject != nullptr)
				draggedGameobject->changeParent(gameobject);
		}
		ImGui::EndDragDropTarget();
	}
	else
	{
		/*ImVec2 a = { ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y };
		ImVec2 b = { ImGui::GetItemRectMax().x, ImGui::GetItemRectMin().y };
		ImGui::DrawLine(a, b);*/
	}
	
	if (ImGui::IsItemClicked(0))
	{
		App->scene_intro->selectGameObject(gameobject);
	}
	if (opened)
	{
		if (gameobject->childs.size() > 0)
		{
			for (std::list<GameObject*>::iterator it_c = gameobject->childs.begin(); it_c != gameobject->childs.end(); it_c++)
			{
				FillTree((*it_c));
			}
		}		
		ImGui::TreePop();
	}	

	if (!gameobject->active)
	{
		ImGui::PopStyleColor();
	}
}
