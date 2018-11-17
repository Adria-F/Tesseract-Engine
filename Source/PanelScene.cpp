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
	

	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 85)/2);
	if (ImGui::ArrowButton("play", ImGuiDir_Right))
	{
		if (!gamePaused)
		{
			if (!App->GameMode)
			{
				App->Save();
				App->GameMode = true;
			}
			else
			{
				App->Load();
				App->GameMode = false;
			}
		}
		else
		{
			App->game_timer.Start();
			gamePaused = false;
		}
	}

	ImGui::SameLine();

	if ( ImGui::Button("||", {23,19}) && App->GameMode )
	{
		App->game_timer.PauseTimer();
		gamePaused = true;
	}

	ImGui::PopStyleColor(2);
	ImGui::SameLine();

	if (!App->GameMode)
		ImGui::PushStyleColor(ImGuiCol_Button, { 1.0f,1.0f,1.0f,0.2f });
	else
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.5f,0.5f,0.95f,0.7f });
	
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.0f,1.0f,1.0f,0.2f });

	if ( ImGui::Button("->", { 23,19 }) && App->GameMode )
	{
		
	}
	ImGui::PopStyleColor(2);

	char sec[64], min[64];
	if (App->GameMode)
	{
		int nsec= (int)((App->game_timer.ReadTime() / 1000) % 60);
		int nmin= (int)((App->game_timer.ReadTime() / 60000) % 3600);

		//milisec = std::to_string(App->game_timer.ReadTime() /1000);
		sprintf(sec,"%02d",nsec);
		sprintf(min, "%02d", nmin);
	}
	else
	{
		sprintf(sec, "00");
		sprintf(min, "00");
	}

	std::string smin = min ;
	std::string ssec = sec;
	std::string total = smin +":"+ ssec;

	ImGui::Text(total.c_str());
	ImGui::EndMenuBar();

	

	ImGui::End();
}
