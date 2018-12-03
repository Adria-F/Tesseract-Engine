#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleGUI.h"
#include "ModuleScene.h"
#include "ModuleSceneLoader.h"
#include "PanelScene.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
#include "ComponentCamera.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

PanelScene::PanelScene(const char* name) : Panel(name)
{
	active = true;
	gamePaused = false;
	step = false;
	gametimer = 0;
}


PanelScene::~PanelScene()
{
}

void PanelScene::Draw()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		flags |= ImGuiWindowFlags_NoMove;
	ImGui::Begin(name.c_str(), &active, flags);

	ImVec2 newSize = ImGui::GetWindowSize();
	if (newSize != size)
	{
		resizedLastFrame = true;
		size = newSize;
		float newAR = size.x / size.y;
		App->camera->camera->setAspectRatio(newAR);
		App->renderer3D->changedFOV = true;
	}
	else
		resizedLastFrame = false;

	ImGui::SetCursorPos({ 0,0 });
	App->gui->sceneX = ImGui::GetCursorPosX() + ImGui::GetWindowPos().x;
	App->gui->sceneY = ImGui::GetCursorPosY() + ImGui::GetWindowPos().y;
	App->gui->sceneW = size.x;
	App->gui->sceneH = size.y;


	ImGui::Image((ImTextureID)App->renderer3D->renderedTexture, { (float)size.x, (float)size.y }, { 0,1 }, { 1,0 });
	
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) != KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_MIDDLE) != KEY_REPEAT)
		App->gui->hoveringScene = ImGui::IsMouseHoveringWindow();

	ImGuizmo::SetDrawlist();

	if (App->scene_intro->selected_GO)
		App->scene_intro->DrawGuizmo(App->scene_intro->guizmoOperation);

	ImGui::BeginMenuBar();
	
	if(!App->GameMode)
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.95f,0.5f,0.0f,0.7f });
	else
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.5f,0.5f,0.95f,0.7f });

	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.0f,1.0f,1.0f,0.2f });
	
	if (App->GameMode)
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.65f,0.7f });

	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 85)/2);
	if (ImGui::ArrowButton("play", ImGuiDir_Right))
	{
		gametimer = 0;

		if (!App->GameMode)
		{
			App->Save();
			App->GameMode = true;
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.65f,0.7f });
		}
		else
		{
			App->Load(); //Game mode is set to false after loading the virtual scene			
			App->GamePaused = false;
		}
	}
	if (App->GameMode)
		ImGui::PopStyleColor();

	ImGui::SameLine();

	if (App->GamePaused)
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.65f,0.7f });

	if ( ImGui::Button("||", {23,19}) && App->GameMode )
	{
		if (!App->GamePaused)
		{
			App->GamePaused = true;
			App->game_timer.PauseTimer();
			ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.65f,0.7f });
		}
		else
		{
			App->GamePaused = false;
			App->game_timer.Start();
			ImGui::PopStyleColor();
		}
	}
	if (App->GamePaused)
		ImGui::PopStyleColor();

	ImGui::PopStyleColor(2);
	ImGui::SameLine();

	if (!App->GameMode)
		ImGui::PushStyleColor(ImGuiCol_Button, { 1.0f,1.0f,1.0f,0.2f });
	else
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.5f,0.5f,0.95f,0.7f });
	
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.0f,1.0f,1.0f,0.2f });
	if (App->GameMode && !App->GamePaused && step == true)
	{
		App->GamePaused = true;
		step = false;
	}

	if (App->GameMode)
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f,0.1f,0.65f,0.7f });
	if ( ImGui::Button("->", { 23,19 }) && App->GameMode && App->GamePaused)
	{
		App->GamePaused = false;
		step = true;
	}
	if (App->GameMode)
		ImGui::PopStyleColor();
	ImGui::PopStyleColor(2);

	char sec[64], min[64];

	float framerelation = (float)App->getFramerateCap()/(float)App->getGameFramerateCap();

	gametimer += App->game_dt/(framerelation*framerelation);
	int nsec= ((int)gametimer) % 60;
	int nmin= ((int)gametimer/60);

	sprintf(sec,"%02d",nsec);
	sprintf(min, "%02d", nmin);	

	std::string smin = min ;
	std::string ssec = sec;
	std::string total = smin +":"+ ssec;

	ImGui::Text(total.c_str());
	ImGui::EndMenuBar();

	

	ImGui::End();
}
