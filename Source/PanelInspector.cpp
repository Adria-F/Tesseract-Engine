#include "PanelInspector.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentTransformation.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "ComponentCamera.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

PanelInspector::PanelInspector(const char* name): Panel(name)
{
	active = true;
}

PanelInspector::~PanelInspector()
{
}

void PanelInspector::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysVerticalScrollbar);

	if (App->scene_intro->selected_GO != nullptr)
	{
		GameObject* go = App->scene_intro->selected_GO;
		char name[120];
		strcpy_s(name, 120, go->name.c_str());
		ImGui::PushID("GO name");
		ImGui::PushItemWidth(150);
		if (ImGui::InputText("", name, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			go->name = name;		
		ImGui::PopItemWidth();
		ImGui::PopID();
		ImGui::Checkbox("Active", &go->active);
		ImGui::SameLine();
		if (ImGui::Checkbox("Static", &go->isStatic))
		{
			App->scene_intro->StartQuadTree();
		}
		ImGui::Separator();

		go->DrawComponentsUI();

		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) / 2);
		ImGui::Button("Add Component", { 300, 25.0f });

		if (addComponent)
		{
			if (ImGui::BeginPopupContextItem("Add Component", 0))
			{
				if (go->GetComponent(MESH) == nullptr)
					if (ImGui::Selectable("Mesh Component"))
						go->AddComponent(MESH);
				if (go->GetComponent(MATERIAL) == nullptr)
					if (ImGui::Selectable("Material Component"))
						go->AddComponent(MATERIAL);
				/*if (go->GetComponent(CAMERA) == nullptr)
					if (ImGui::Selectable("Camera Component"))
						go->AddComponent(CAMERA);*/

				ImGui::EndPopup();
			}
		}
		else
			addComponent = true;
	}

	ImGui::End();
}
