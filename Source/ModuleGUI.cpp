#include "Application.h"
#include "ModuleGUI.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl.h"
#include "ImGui\imgui_impl_opengl2.h"

#include "Panel.h"
#include "PanelConfiguration.h"
#include "PanelConsole.h"
#include "PanelAbout.h"
#include "PanelHardwareInfo.h"
#include "PanelElements.h"
#include "PanelInspector.h"
#include "PanelScene.h"
#include "PanelAssets.h"
#include "PanelHierarchy.h"

#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleTextures.h"

#include "ModuleSceneLoader.h"

ModuleGUI::ModuleGUI(bool start_enabled) : Module(start_enabled)
{
}

ModuleGUI::~ModuleGUI()
{
}

bool ModuleGUI::Init(JSON_File* document)
{

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();

	panels.push_back(hardwareInfo = new PanelHardwareInfo("Hardware Info"));
	
	panels.push_back(console = new PanelConsole("Console"));
	
	panels.push_back(inspector = new PanelInspector("Inspector"));

	panels.push_back(configuration = new PanelConfiguration("Configuration"));
	
	panels.push_back(about = new PanelAbout("About"));

	panels.push_back(ShapeElements = new PanelElements("3D Elements"));

	panels.push_back(Scene = new PanelScene("Scene"));

	panels.push_back(assets = new PanelAssets("Assets"));

	panels.push_back(hierarchy = new PanelHierarchy("Assets"));

	//OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return true;
}

bool ModuleGUI::Start()
{	
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = { 0.1f,0.1f,0.1f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg] = { 0.05f, 0.05f,0.05f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = {0.05f, 0.05f,0.05f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = { 0.3f, 0.3f,0.3f,1.0f };

	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_Tab] = { 0.7f,0.3f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_TabHovered] = { 0.8f,0.35f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_TabUnfocused] = { 0.5f,0.2f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_TabUnfocusedActive] = { 0.65f,0.25f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_TabActive] = { 0.9f,0.45f,0.0f,1.0f };

	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_Header] = { 0.8f,0.37f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_HeaderActive] = { 1.0f,0.6f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_HeaderHovered] = { 0.95f,0.5f,0.0f,1.0f };

	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_Button] = { 0.9f,0.45f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ButtonActive] = { 0.8f,0.37f,0.0f,1.0f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_ButtonHovered] = { 0.95f,0.5f,0.0f,1.0f };

	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_FrameBg] = { 1.0f,0.6f,0.0f,0.1f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_FrameBgActive] = { 0.8f,0.37f,0.0f,0.5f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_FrameBgHovered] = { 0.95f,0.5f,0.0f,0.5f };

	ImGui::GetStyle().WindowMinSize = { 125, 100 };

	folder = App->textures->loadIcon("Icons/folder.png");
	file = App->textures->loadIcon("Icons/file.png");

	return true;
}

update_status ModuleGUI::PreUpdate(float dt)
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize({(float)App->window->width, (float)App->window->height});
	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", 0, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_RenderWindowBg);

	return update_status(UPDATE_CONTINUE);
}

update_status ModuleGUI::Update(float dt)
{
	update_status status= UPDATE_CONTINUE;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{	
			if (ImGui::MenuItem("Save", "ctrl+S"))
				App->SaveGame();
			if (ImGui::MenuItem("Load"))
				App->LoadGame();
			if (ImGui::MenuItem("Save Scene"))
				App->scene_loader->saveScene("sceneTest");
			if (ImGui::MenuItem("Load Scene"))
				App->scene_loader->loadScene("sceneTest");
			if (ImGui::MenuItem("New Scene"))
				App->scene_intro->newScene();
			if (ImGui::MenuItem("Close", "ESC"))
			{
				ImGui::EndMenu(); //End menu
				ImGui::EndMainMenuBar(); //End main menu
				ImGui::End(); //End main docking window
				ImGui::EndFrame(); //End frame
				return UPDATE_STOP;
			}

			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("3D Tools"))
		{
			if (ImGui::MenuItem("Basic 3D Shapes", NULL, ShapeElements->isActive()))
				ShapeElements->toggleActive();
			if (ImGui::MenuItem("Mesh Properties", NULL, inspector->isActive()))
				inspector->toggleActive();
			if(ImGui::MenuItem("Game Objects",NULL,hierarchy->isActive()))
				hierarchy->toggleActive();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::MenuItem("Console", NULL, console->isActive()))
				console->toggleActive();
			if (ImGui::MenuItem("Config", NULL, configuration->isActive()))
				configuration->toggleActive();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Hardware Info",NULL, hardwareInfo->isActive()))
				hardwareInfo->toggleActive();
			if (ImGui::MenuItem("About", NULL, about->isActive()))
				about->toggleActive();
			if (ImGui::MenuItem("GitHub Repository"))
				App->RequestBrowser("https://github.com/Adria-F/Engines");
			ImGui::EndMenu();
		}
	
	}
	ImGui::EndMainMenuBar();

	return status;
}

update_status ModuleGUI::PostUpdate(float dt)
{
	

	return update_status(UPDATE_CONTINUE);
}

bool ModuleGUI::CleanUp()
{
	for (std::list<Panel*>::iterator it_p = panels.begin(); it_p != panels.end(); it_p++)
	{
		RELEASE((*it_p));
	}

	panels.clear();
	console = nullptr;

	ImGui_ImplSDL2_Shutdown();
	ImGui_ImplOpenGL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ModuleGUI::logFPS(float fps, float ms)
{
	if (configuration != nullptr)
		configuration->addFPS(fps, ms);
}

void ModuleGUI::logMemory(float memory)
{
	if (configuration != nullptr)
		configuration->addMemory(memory);
}

void ModuleGUI::AddLog(const char* log)
{
	if(App->gui && console!=nullptr)
		console->AddLog(log);
}

void ModuleGUI::handleInput(SDL_Event * event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

bool ModuleGUI::Save(JSON_File* document)const
{
	JSON_Value* gui = document->createValue();
	gui->addString("name", "gui");
	document->addValue("gui", gui);

	return true;
}
bool ModuleGUI::Load(JSON_File* document) {
	return true;
}

void ModuleGUI::Draw()
{
	for (std::list<Panel*>::iterator it_p = panels.begin(); it_p != panels.end(); it_p++)
	{
		if ((*it_p)->isActive())
		{
			ImGui::SetNextWindowSizeConstraints({ 10,10 }, { (float)App->window->width, (float)App->window->height });
			(*it_p)->Draw();
		}
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

bool ModuleGUI::isMouseOnGUI() const
{
	return ImGui::GetIO().WantCaptureMouse && !hoveringScene;
}
