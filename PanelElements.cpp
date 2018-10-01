#include "Application.h"
#include "ModuleSceneIntro.h"
#include "PanelElements.h"
#include "Primitive.h"
#include "ImGui\imgui.h"

PanelElements::PanelElements(const char* name, float posX, float posY, float width, float height) : Panel(name, posX, posY, width, height)
{
	active = true;
}

PanelElements::~PanelElements()
{
}


void PanelElements::Draw() 
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing);

	if (ImGui::CollapsingHeader("Box", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat3("Box Center", Box_C);
		ImGui::DragFloat3("Box Size", Box_S);

		if (ImGui::Button("Create Box"))
		{
			App->scene_intro->ShapesToDraw.push_back(new MCube(Box_S[0], Box_S[1], Box_S[2], { Box_C[0], Box_C[1], Box_C[2]}));
		}
	}
	if (ImGui::CollapsingHeader("Sphere"))
	{
		ImGui::InputInt("Sphere Rings",&S_Ring);
		ImGui::InputInt("Sphere Sectors", &S_Sectors);

		ImGui::NewLine();
		ImGui::DragFloat("Shpere Radius", &Sphere_R);
		ImGui::DragFloat3("Sphere Center", Sphere_C);
		

		if (ImGui::Button("Create Sphere"))
		{
			App->scene_intro->ShapesToDraw.push_back(new MSphere(Sphere_R, S_Ring, S_Sectors, { Sphere_C[0], Sphere_C[1], Sphere_C[2] }));
		}

	}
	if (ImGui::CollapsingHeader("Cylinder"))
	{
		ImGui::InputInt("Cylinder Rings", &C_Ring);
		ImGui::InputInt("Cylinder Sectors", &C_Sectors);

		ImGui::NewLine();
		ImGui::DragFloat("Cylinder Radius", &Cylinder_R);
		ImGui::DragFloat("Cylinder Height", &Cylinder_H);
		ImGui::DragFloat3("Cylinder Center", Cylinder_C);


		if (ImGui::Button("Create Cylinder"))
		{
			App->scene_intro->ShapesToDraw.push_back(new MCylinder(Cylinder_R, Cylinder_H, C_Ring, C_Sectors, { Cylinder_C[0], Cylinder_C[1], Cylinder_C[2] }));
		}
	}

	ImGui::End();
}