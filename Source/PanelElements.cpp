#include "Application.h"
#include "ModuleScene.h"
#include "PanelElements.h"
#include "Primitive.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

PanelElements::PanelElements(const char* name) : Panel(name)
{
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
			//TODO it creates a gameObject
			//App->scene_intro->ShapesToDraw.push_back(new MCube(Box_S[0], Box_S[1], Box_S[2], { Box_C[0], Box_C[1], Box_C[2]}));
		}
	}
	if (ImGui::CollapsingHeader("Sphere"))
	{
		if (S_Ring < 4)
			S_Ring = 4;
		ImGui::InputInt("Sphere Rings",&S_Ring, 1.0f, 2.0f);
		if (S_Sectors < 4)
			S_Sectors = 4;
		ImGui::InputInt("Sphere Sectors", &S_Sectors, 1.0f, 6.0f);

		ImGui::NewLine();
		if (Sphere_R <= 0)
			Sphere_R = 1;
		ImGui::DragFloat("Shpere Radius", &Sphere_R, 1.0f, 1.0f, 500.0f);
		ImGui::DragFloat3("Sphere Center", Sphere_C);
		

		if (ImGui::Button("Create Sphere"))
		{
			//TODO it creates a gameObject
			//App->scene_intro->ShapesToDraw.push_back(new MSphere(Sphere_R, S_Ring, S_Sectors, { Sphere_C[0], Sphere_C[1], Sphere_C[2] }));
		}

	}
	if (ImGui::CollapsingHeader("Cylinder"))
	{
		if (Cy_Ring < 2)
			Cy_Ring = 2;
		ImGui::InputInt("Cylinder Rings", &Cy_Ring, 1.0f, 2.0f);
		if (Cy_Sectors < 10)
			Cy_Sectors = 10;
		ImGui::InputInt("Cylinder Sectors", &Cy_Sectors, 1.0f, 6.0f);

		ImGui::NewLine();
		if (Cylinder_R <= 0)
			Cylinder_R = 1;
		ImGui::DragFloat("Cylinder Radius", &Cylinder_R, 1.0f, 1.0f, 500.0f);
		if (Cylinder_H <= 0)
			Cylinder_H = 1;
		ImGui::DragFloat("Cylinder Height", &Cylinder_H, 1.0f, 1.0f, 500.0f);
		ImGui::DragFloat3("Cylinder Center", Cylinder_C);


		if (ImGui::Button("Create Cylinder"))
		{
			//TODO it creates a gameObject
			//App->scene_intro->ShapesToDraw.push_back(new MCylinder(Cylinder_R, Cylinder_H, Cy_Ring, Cy_Sectors, { Cylinder_C[0], Cylinder_C[1], Cylinder_C[2] }));
		}
	}
	if (ImGui::CollapsingHeader("Capsule"))
	{
		if (Ca_Ring < 4)
			Ca_Ring = 4;
		ImGui::InputInt("Capsule Rings", &Ca_Ring, 1.0f, 2.0f);
		if (Ca_Sectors < 10)
			Ca_Sectors = 10;
		ImGui::InputInt("Capsule Sectors", &Ca_Sectors, 1.0f, 6.0f);

		ImGui::NewLine();
		if (Capsule_R <= 0)
			Capsule_R = 1;
		ImGui::DragFloat("Capsule Radius", &Capsule_R, 1.0f, 1.0f, 500.0f);
		if (Capsule_H <= 0)
			Capsule_H = 1;
		ImGui::DragFloat("Capsule Height", &Capsule_H, 1.0f, 1.0f, 500.0f);
		ImGui::DragFloat3("Capsule Center", Capsule_C);


		if (ImGui::Button("Create Capsule"))
		{
			//TODO it creates a gameObject
			//App->scene_intro->ShapesToDraw.push_back(new MCapsule(Capsule_R, Capsule_H, Ca_Ring, Ca_Sectors, { Capsule_C[0], Capsule_C[1], Capsule_C[2] }));
		}
	}
	if (ImGui::CollapsingHeader("Frustum"))
	{
		if (Frustum_Ds <= 0)
			Frustum_Ds = 1;
		ImGui::DragFloat("Frustum Distance", &Frustum_Ds, 1.0f, 1.0f, 500.0f);

		ImGui::NewLine();
		if (Frustum_W <= 0)
			Frustum_W = 1;
		ImGui::DragFloat("Frustum Width", &Frustum_W, 1.0f, 1.0f, 500.0f);
		if (Frustum_H <= 0)
			Frustum_H = 1;
		ImGui::DragFloat("Frustum Height", &Frustum_H, 1.0f, 1.0f, 500.0f);
		if (Frustum_D <= 0)
			Frustum_D = 1;
		ImGui::DragFloat("Frustum Depth", &Frustum_D, 1.0f, 1.0f, 500.0f);

		ImGui::NewLine();
		ImGui::DragFloat3("Frustum Origin", Frustum_C);

		if (ImGui::Button("Create Frustum"))
		{
			//TODO it creates a gameObject
			//App->scene_intro->ShapesToDraw.push_back(new MFrustum(Frustum_Ds, Frustum_D, Frustum_W, Frustum_H, { Frustum_C[0], Frustum_C[1], Frustum_C[2] }));
		}
	}

	ImGui::End();
}