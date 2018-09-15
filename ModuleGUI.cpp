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

	ImGui::SetNextWindowPos({ 0,0 });
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin("Test Menu", 0, window_flags);
	ImGui::SetWindowSize({ 85, 35 });
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(255, 0, 0, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(150, 0, 0, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(100, 0, 0, 255));
	if (ImGui::Button("Close App"))
		status = UPDATE_STOP;
	ImGui::PopStyleColor(3);
	ImGui::End();
	ImGui::ShowDemoWindow();

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
