#include "ModuleGUI.h"
#include "Application.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl.h"
#include "ImGui\imgui_impl_opengl2.h"
#include "Panel.h"
#include "PanelConfiguration.h"
#include "PanelConsole.h"
#include "PanelAbout.h"
#include "PanelHardwareInfo.h"

ModuleGUI::ModuleGUI(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleGUI::~ModuleGUI()
{
}

bool ModuleGUI::Init()
{
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();

	return true;
}

bool ModuleGUI::Start()
{	
	panels.push_back(hardwareInfo = new PanelHardwareInfo("Hardware Info"));
	panels.push_back(console = new PanelConsole("Console"));
	panels.push_back(configuration = new PanelConfiguration("Configuration"));
	panels.push_back(about = new PanelAbout("About"));

	return true;
}

update_status ModuleGUI::PreUpdate(float dt)
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return update_status(UPDATE_CONTINUE);
}

update_status ModuleGUI::Update(float dt)
{
	update_status status= UPDATE_CONTINUE;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{		
			if (ImGui::MenuItem("Close", "ESC"))
				status = UPDATE_STOP;

			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Hardware Info"))
				hardwareInfo->toggleActive();
			if (ImGui::MenuItem("Console"))
				console->toggleActive();
			if (ImGui::MenuItem("Config"))
				configuration->toggleActive();
			if (ImGui::MenuItem("About"))
				about->toggleActive();
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	for (std::list<Panel*>::iterator it_p = panels.begin(); it_p != panels.end(); it_p++)
	{
		if ((*it_p)->isActive())
		{
			ImGui::SetNextWindowPos({ (*it_p)->posX, (*it_p)->posY });
			ImGui::SetNextWindowSize({ (*it_p)->width, (*it_p)->height });
			(*it_p)->Draw();
		}
	}

	/*
	//Object Generator-------------------------------------------
	if (Sphere_tb)
	{
	
		ImGui::SetNextWindowPos({ 250,20 });
		
		ImGui::Begin("Intersection Test", &Sphere_tb);

		ImGui::Text("Here you can test the collision between two Spheres");
		ImGui::Text("Create Sphere");

		ImGui::DragFloat4("Position and radius A", aux_A);
		
		ImGui::Text("");

		ImGui::DragFloat4("Position  and radius B", aux_B);
		ImGui::Text("Click to test");
		if (ImGui::Button("Test"))
		{
			App->scene_intro->S_Test_A.pos = { aux_A[0],aux_A[1],aux_A[2] };
			App->scene_intro->S_Test_A.r = aux_A[3];
			App->scene_intro->S_Test_B.pos = { aux_B[0],aux_B[1],aux_B[2] };
			App->scene_intro->S_Test_B.r = aux_B[3];
		}

		if (App->scene_intro->S_Test_A.Intersects(App->scene_intro->S_Test_B))
		{
			ImGui::TextColored({ 0,1,0,1 }, "Intersection");
			LOG("Intersection");
		}
		else
		{
			ImGui::TextColored({ 1,0,0,1 }, "No Intersection");
			LOG("No Intersection");
		}
		
		ImGui::End();		
	}

	if (AABB_tb)
	{

		ImGui::SetNextWindowPos({ 250,20 });

		ImGui::Begin("Intersection Test", &AABB_tb);

		ImGui::Text("Here you can test the collision between two AABB");
		ImGui::Text("Create AABB");

		ImGui::DragFloat3("Min pos A", Min_aux_A);
		ImGui::DragFloat3("Max pos A", Max_aux_A);

		ImGui::Text("");

		ImGui::DragFloat3("Min pos B", Min_aux_B);
		ImGui::DragFloat3("Max pos B", Max_aux_B);

		ImGui::Text("Click to test");
		ImGui::PushID("Test2");
		if (ImGui::Button("Test"))
		{
			App->scene_intro->AABB_Test_A.minPoint = { Min_aux_A[0] ,Min_aux_A[1] ,Min_aux_A[2] };
			App->scene_intro->AABB_Test_A.maxPoint = { Max_aux_A[0] ,Max_aux_A[1] ,Max_aux_A[2] };
			App->scene_intro->AABB_Test_B.minPoint = { Min_aux_B[0] ,Min_aux_B[1] ,Min_aux_B[2] };
			App->scene_intro->AABB_Test_B.maxPoint = { Max_aux_B[0] ,Max_aux_B[1] ,Max_aux_B[2] };
		}
		ImGui::PopID();

		if (App->scene_intro->AABB_Test_A.Intersects(App->scene_intro->AABB_Test_B))
		{
			ImGui::TextColored({ 0,1,0,1 }, "Intersection");
			LOG("Intersection");
		}
		else
		{
			ImGui::TextColored({ 1,0,0,1 }, "No Intersection");
			LOG("No Intersection");
		}

		ImGui::End();
	}
	
	*/

	return status;
}

update_status ModuleGUI::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	return update_status(UPDATE_CONTINUE);
}

bool ModuleGUI::CleanUp()
{
	ImGui_ImplSDL2_Shutdown();
	ImGui_ImplOpenGL2_Shutdown();

	panels.clear();
	console = nullptr;

	return true;
}

void ModuleGUI::logFPS(float fps, float ms)
{
	if (configuration != nullptr)
		configuration->addFPS(fps, ms);
}
