#include "Application.h"
#include "ModuleGUI.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_sdl.h"
#include "ImGui\imgui_impl_opengl2.h"
#include "ImGuizmo\ImGuizmo.h"

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
#include "PanelFileDialog.h"
#include "PanelResourcesList.h"
#include "PanelGame.h"
#include "PanelAnimation.h"

#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleTextures.h"

#include "ModuleSceneLoader.h"
#include "ModuleFileSystem.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

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
	io.IniFilename = "Settings/imgui.ini";

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();

	panels.push_back(hardwareInfo = new PanelHardwareInfo("Hardware Info"));
	
	panels.push_back(console = new PanelConsole("Console"));

	panels.push_back(configuration = new PanelConfiguration("Configuration"));

	panels.push_back(inspector = new PanelInspector("Inspector"));
	
	panels.push_back(about = new PanelAbout("About"));

	panels.push_back(ShapeElements = new PanelElements("3D Elements"));

	panels.push_back(Scene = new PanelScene("Scene"));

	panels.push_back(assets = new PanelAssets("Assets"));

	panels.push_back(hierarchy = new PanelHierarchy("Hierarchy"));

	panels.push_back(fileDialog = new PanelFileDialog("Files"));

	panels.push_back(resourcesList = new PanelResourcesList("Resources"));
	
	panels.push_back(GameScene = new PanelGame("Game"));
	
	panels.push_back(animations = new PanelAnimation("Animations"));

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

	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_FrameBg] = { 1.0f,0.6f,0.0f,0.2f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_FrameBgActive] = { 0.8f,0.37f,0.0f,0.5f };
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_FrameBgHovered] = { 0.95f,0.5f,0.0f,0.5f };

	ImGui::GetStyle().WindowMinSize = { 125, 100 };

	folder = App->textures->loadIcon("Icons/folder.png");
	file = App->textures->loadIcon("Icons/file.png");
	small_folder = App->textures->loadIcon("Icons/small_folder.png");

	return true;
}

update_status ModuleGUI::PreUpdate(float dt)
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize({(float)App->window->width, (float)App->window->height});
	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", 0, window_flags);
	ImGui::PopStyleVar(3);

	drawToolsMenu();

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
			if (ImGui::MenuItem("Save Scene"))
				App->scene_intro->wantToSaveScene();
			if (ImGui::MenuItem("Load Scene"))
				App->scene_intro->wantToLoadScene();
			if (ImGui::MenuItem("New Scene"))
			{
				App->scene_intro->newScene();
				App->scene_intro->AddCamera();
			}
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

		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Basic 3D Shapes", NULL, ShapeElements->isActive()))
				ShapeElements->toggleActive();
			if (ImGui::MenuItem("Inspector", NULL, inspector->isActive()))
				inspector->toggleActive();
			if(ImGui::MenuItem("Game Objects",NULL,hierarchy->isActive())) 
				hierarchy->toggleActive();
			if (ImGui::MenuItem("Assets", NULL, assets->isActive()))
				assets->toggleActive();
			if (ImGui::MenuItem("Animation", NULL, animations->isActive()))
				animations->toggleActive();
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
	ImGui::Begin("Scene");
	if (ImGui::IsWindowAppearing())
		lastFocusScene = true;
	ImGui::End();

	ImGui::Begin("Game");
	if (ImGui::IsWindowAppearing())
		lastFocusScene = false;
	ImGui::End();

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

void ModuleGUI::logGameFPS(float fps, float ms)
{
	if (configuration != nullptr)
		configuration->addGameFPS(fps, ms);
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

void ModuleGUI::SaveDialogAt(const char * path, const char* exclusiveExtension)
{
	fileDialog->SaveAt(path, exclusiveExtension);
}

void ModuleGUI::LoadDialogAt(const char * path, const char* exclusiveExtension)
{
	fileDialog->LoadAt(path, exclusiveExtension);
}

void ModuleGUI::startResourceList(ResType type, int x, int y, Component* callback, const char* tag)
{
	resourcesList->startList(type, x, y, callback, tag);
}

void ModuleGUI::saveDefaultImgui()
{
	LOG("imgui.ini file is missing, so there is no information about the UI layout");
	LOG("All windows have been moved to the default position and size, please consider docking them and setting them as desired");

	for (std::list<Panel*>::iterator it_p = panels.begin(); it_p != panels.end(); it_p++)
	{
		ImGui::SetNextWindowSizeConstraints({ 10,10 }, { (float)App->window->width, (float)App->window->height });
		(*it_p)->Draw(); //Draw every panel, so it detects it and change its position
	}

	float horizontalFactor = (float)App->window->width / SCREEN_WIDTH;
	float verticalFactor = (float)App->window->height / SCREEN_HEIGHT;

	ImGui::GetStyle().WindowMinSize = { 125* horizontalFactor, 100* verticalFactor };

	ImGui::SetWindowPos("Hardware Info", { 0* horizontalFactor,646* verticalFactor });
	ImGui::SetWindowSize("Hardware Info", { 1083* horizontalFactor, 118 * verticalFactor });

	ImGui::SetWindowPos("Console", { 0* horizontalFactor,810 * verticalFactor });
	ImGui::SetWindowSize("Console", { 1292* horizontalFactor, 140 * verticalFactor });

	ImGui::SetWindowPos("Configuration", { 1291* horizontalFactor,19 * verticalFactor });
	ImGui::SetWindowSize("Configuration", { 290* horizontalFactor, 468 * verticalFactor });

	ImGui::SetWindowPos("About", { 0* horizontalFactor,810 * verticalFactor });
	ImGui::SetWindowSize("About", { 1292* horizontalFactor, 140 * verticalFactor });

	ImGui::SetWindowPos("3D Elements", { 0* horizontalFactor,19 * verticalFactor });
	ImGui::SetWindowSize("3D Elements", { 214* horizontalFactor, 419 * verticalFactor });

	ImGui::SetWindowPos("DockSpace", { 0* horizontalFactor,0 * verticalFactor });
	ImGui::SetWindowSize("DockSpace", { 1580 * horizontalFactor, 950 * verticalFactor });

	ImGui::SetWindowPos("Scene", { 214* horizontalFactor,19 * verticalFactor });
	ImGui::SetWindowSize("Scene", { 1079 * horizontalFactor, 790 * verticalFactor });

	ImGui::SetWindowPos("Assets", { 0* horizontalFactor,438 * verticalFactor });
	ImGui::SetWindowSize("Assets", { 214* horizontalFactor, 373 * verticalFactor });

	ImGui::SetWindowPos("Game Objects", { 1292* horizontalFactor,487 * verticalFactor });
	ImGui::SetWindowSize("Game Objects", { 286* horizontalFactor, 463 * verticalFactor });

	ImGui::SetWindowPos("Inspector", { 1291 * horizontalFactor,19 * verticalFactor });
	ImGui::SetWindowSize("Inspector", { 290 * horizontalFactor, 468 * verticalFactor });

	ImGui::SetWindowPos("Save", { 314 * horizontalFactor,121 * verticalFactor });
	ImGui::SetWindowSize("Save", { 564 * horizontalFactor, 356 * verticalFactor });

	ImGui::SetWindowPos("Load", { 314 * horizontalFactor,121 * verticalFactor });
	ImGui::SetWindowSize("Load", { 564 * horizontalFactor, 356 * verticalFactor });

	ImGui::SetWindowPos("Resources", { 847 * horizontalFactor,19 * verticalFactor });
	ImGui::SetWindowSize("Resources", { 204 * horizontalFactor, 288 * verticalFactor });

	std::string path = SETTINGS_FOLDER;
	path += "imgui.ini";
	ImGui::SaveIniSettingsToDisk(path.c_str());
}

void ModuleGUI::Draw()
{
	if (!App->fileSystem->fileExists("imgui.ini"))
		saveDefaultImgui();

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

bool ModuleGUI::sceneVisible() const
{
	return Scene->isActive() && (!sceneAndGameDocked() || lastFocusScene);
}

bool ModuleGUI::gameVisible() const
{
	return GameScene->isActive() && (!sceneAndGameDocked() || !lastFocusScene);
}

bool ModuleGUI::sceneAndGameDocked() const
{
	ImGui::Begin("Scene");
	int scene = ImGui::GetWindowDockId();
	ImGui::End();
	ImGui::Begin("Game");
	int game = ImGui::GetWindowDockId();
	ImGui::End();

	return scene == game && scene != 0;
}

void ModuleGUI::drawToolsMenu()
{
	ImGui::BeginMenuBar();

	bool changedGameMode = false;
	bool changedPause = false;
	bool doStep = false;
	static bool step = false;

	if (!App->inGameMode())
	{
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.95f,0.5f,0.0f,0.7f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.95f,0.3f,0.0f,0.5f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f,0.37f,0.0f,1.0f });
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.5f,0.5f,0.95f,0.7f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f,0.3f,0.95f,0.5f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f,0.1f,0.65f,0.7f });
	}

	if (App->inGameMode())
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.65f,0.7f });

	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 85) / 2);
	changedGameMode = ImGui::ArrowButton("play", ImGuiDir_Right);

	if (App->inGameMode())
		ImGui::PopStyleColor();

	ImGui::SameLine();

	if (App->isGamePaused() || step)
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.65f,0.7f });

	changedPause = ImGui::Button("||", { 23,19 });

	if (App->isGamePaused() || step)
		ImGui::PopStyleColor();

	ImGui::SameLine();


	if (!App->inGameMode() || !App->isGamePaused())
	{
		ImGui::PushStyleColor(ImGuiCol_Button, { 1.0f,1.0f,1.0f,0.2f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.0f,1.0f,1.0f,0.2f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1.0f,1.0f,1.0f,0.2f });
	}

	doStep = ImGui::Button("->", { 23,19 });

	if (!App->inGameMode() || !App->isGamePaused())
		ImGui::PopStyleColor(3);

	//Buttons actions
	if (changedGameMode)
	{
		if (!App->inGameMode())
		{
			App->Save();
			App->StartGame();
		}
		else
		{
			App->Load(); //Game mode is set to false after loading the virtual scene			
			App->StopGame();
		}
	}
	if (changedPause && App->inGameMode())
	{
		if (!App->isGamePaused())
		{
			App->PauseGame(true);
		}
		else
		{
			App->PauseGame(false);
		}
	}
	if (step)
	{
		App->PauseGame(true);
		step = false;
	}
	if (doStep && App->inGameMode() && App->isGamePaused())
	{
		App->PauseGame(false);
		step = true;
	}

	//Game time
	char sec[64], min[64];

	float framerelation = (float)App->getFramerateCap() / (float)App->getGameFramerateCap();

	float gametimer = App->ReadGameTime()/1000;
	int nsec = ((int)gametimer) % 60;
	int nmin = ((int)gametimer / 60);

	sprintf(sec, "%02d", nsec);
	sprintf(min, "%02d", nmin);

	std::string smin = min;
	std::string ssec = sec;
	std::string total = smin + ":" + ssec;

	ImGui::Text(total.c_str());

	ImGui::PopStyleColor(3);

	ImGui::EndMenuBar();
}

bool ModuleGUI::isMouseOnGUI() const
{
	return ImGui::GetIO().WantCaptureMouse && !hoveringScene;
}

bool ModuleGUI::GUIKeppsKeyboard() const
{
	return ImGui::GetIO().WantTextInput;
}
