#include "Application.h"
#include "ModuleScene.h"
#include "PanelElements.h"
#include "Primitive.h"

PanelElements::PanelElements(const char* name, float posX, float posY, float width, float height, panelAlingnment aligned) : Panel(name, posX, posY, width, height, aligned)
{
	active = true;
}

PanelElements::~PanelElements()
{
}


void PanelElements::Draw() 
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing);

	if (ImGui::CollapsingHeader("Box"))
	{
		ImGui::DragFloat3("Box Center", Box_C);
		ImGui::DragFloat3("Box Size", Box_S, 1.0f, 1.0f);

		if (ImGui::Button("Create Box"))
		{
			App->scene_intro->ShapesToDraw.push_back(new MCube(Box_S[0], Box_S[1], Box_S[2], { Box_C[0], Box_C[1], Box_C[2]}));
		}
	}
	if (ImGui::CollapsingHeader("Sphere"))
	{
		ImGui::InputInt("Sphere Rings",&S_Ring, 1.0f, 2.0f);
		ImGui::InputInt("Sphere Sectors", &S_Sectors, 1.0f, 6.0f);

		ImGui::NewLine();
		ImGui::DragFloat("Shpere Radius", &Sphere_R, 1.0f, 1.0f);
		ImGui::DragFloat3("Sphere Center", Sphere_C);
		

		if (ImGui::Button("Create Sphere"))
		{
			App->scene_intro->ShapesToDraw.push_back(new MSphere(Sphere_R, S_Ring, S_Sectors, { Sphere_C[0], Sphere_C[1], Sphere_C[2] }));
		}

	}
	if (ImGui::CollapsingHeader("Cylinder"))
	{
		ImGui::InputInt("Cylinder Rings", &Cy_Ring, 1.0f, 2.0f);
		ImGui::InputInt("Cylinder Sectors", &Cy_Sectors, 1.0f, 6.0f);

		ImGui::NewLine();
		ImGui::DragFloat("Cylinder Radius", &Cylinder_R, 1.0f, 1.0f);
		ImGui::DragFloat("Cylinder Height", &Cylinder_H, 1.0f, 1.0f);
		ImGui::DragFloat3("Cylinder Center", Cylinder_C);


		if (ImGui::Button("Create Cylinder"))
		{
			App->scene_intro->ShapesToDraw.push_back(new MCylinder(Cylinder_R, Cylinder_H, Cy_Ring, Cy_Sectors, { Cylinder_C[0], Cylinder_C[1], Cylinder_C[2] }));
		}
	}
	if (ImGui::CollapsingHeader("Capsule"))
	{
		ImGui::InputInt("Capsule Rings", &Ca_Ring, 1.0f, 2.0f);
		ImGui::InputInt("Capsule Sectors", &Ca_Sectors, 1.0f, 6.0f);

		ImGui::NewLine();
		ImGui::DragFloat("Capsule Radius", &Capsule_R, 1.0f, 1.0f);
		ImGui::DragFloat("Capsule Height", &Capsule_H, 1.0f, 1.0f);
		ImGui::DragFloat3("Capsule Center", Capsule_C);


		if (ImGui::Button("Create Capsule"))
		{
			App->scene_intro->ShapesToDraw.push_back(new MCapsule(Capsule_R, Capsule_H, Ca_Ring, Ca_Sectors, { Capsule_C[0], Capsule_C[1], Capsule_C[2] }));
		}
	}
	if (ImGui::CollapsingHeader("Frustum"))
	{
		ImGui::DragFloat("Frustum Distance", &Frustum_Ds, 1.0f, 1.0f);

		ImGui::NewLine();
		ImGui::DragFloat("Frustum Width", &Frustum_W, 1.0f, 1.0f);
		ImGui::DragFloat("Frustum Height", &Frustum_H, 1.0f, 1.0f);
		ImGui::DragFloat("Frustum Depth", &Frustum_D, 1.0f, 1.0f);

		ImGui::NewLine();
		ImGui::DragFloat3("Frustum Origin", Frustum_C);

		if (ImGui::Button("Create Frustum"))
		{
			App->scene_intro->ShapesToDraw.push_back(new MFrustum(Frustum_Ds, Frustum_D, Frustum_W, Frustum_H, { Frustum_C[0], Frustum_C[1], Frustum_C[2] }));
		}
	}

	checkMoved();
	ImGui::End();
}