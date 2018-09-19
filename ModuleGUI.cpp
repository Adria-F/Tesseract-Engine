#include "ModuleGUI.h"
#include "Application.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl.h"
#include "ImGui\imgui_impl_opengl2.h"

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
			if (ImGui::MenuItem("Demo Window"))
				demoWindow = !demoWindow;
			
			if (ImGui::MenuItem("Close", "ESC"))
				status = UPDATE_STOP;

			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Util"))
		{
			if (ImGui::MenuItem("RNG"))
				RNGb = !RNGb;
			if (ImGui::MenuItem("Speher intersection"))
				Sphere_tb = !Sphere_tb;
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	if (demoWindow)
	{
		ImGui::ShowDemoWindow();
	}
	
	//Object Generator-------------------------------------------
	if (Sphere_tb)
	{
	
		ImGui::SetNextWindowPos({ 250,20 });
		
		ImGui::Begin("Sphere Test", &Sphere_tb);

		ImGui::Text("Here you can test the collision between two Spheres");
		ImGui::Text("Create an Object");

		ImGui::DragFloat4("Position and radius A", aux_A);
		
		if (ImGui::Button("SAVE"))
		{
			App->scene_intro->S_Test_A.pos = { aux_A[0],aux_A[1],aux_A[2] };
			App->scene_intro->S_Test_A.r = aux_A[4];
		}

		ImGui::Text("");

		ImGui::DragFloat4("Position  and radius B", aux_B);

		if (ImGui::Button("SAVE"))
		{
			App->scene_intro->S_Test_B.pos = { aux_B[0],aux_B[1],aux_B[2] };
			App->scene_intro->S_Test_B.r = aux_B[4];
		}
		ImGui::Text("");

		ImGui::Text("Click to test");
		
		if (ImGui::Button("Test"))
		{
			if (App->scene_intro->S_Test_A.Intersects(App->scene_intro->S_Test_B))
			{
				LOG("Intersection");
			}
			else
			{
				LOG("No Intersection");
			}
		}

		
		ImGui::End();		
	}
	
	//RNG Window-------------------------------------------------
	if (RNGb)
	{
		
		ImGui::SetNextWindowPos({ 0,20 });
		ImGui::SetWindowPos({ 200, 270 });

		ImGui::Begin("RNG based on PCG", &RNGb);
		ImGui::Text("Here you can test the two ");
		ImGui::Text("basic functions of the RNG");

		ImGui::NewLine();
		ImGui::Text("Get a random float between");
		ImGui::Text("0 and 1");
		if (ImGui::Button("Get"))
		{
			firstR = GET_RANDOM();
		}
		ImGui::SameLine(50.0f);
		ImGui::Text("%f", firstR);

		ImGui::NewLine();
		ImGui::Text("Get a random int between");
		ImGui::Text("two numbers");

		ImGui::SliderInt("MIN", &min, -500, max);
		ImGui::SliderInt("MAX", &max, min, 500);
		ImGui::PushID("Get2");
		if (ImGui::Button("Get"))
		{
			secondR = GET_RANDOM_BETWEEN(min, max);
		}
		ImGui::PopID();
		ImGui::SameLine(50.0f);
		ImGui::Text("%d", secondR);
		ImGui::End();
	}

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

	return true;
}