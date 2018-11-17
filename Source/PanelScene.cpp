#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleGUI.h"
#include "ModuleScene.h"
#include "ModuleSceneLoader.h"
#include "PanelScene.h"
#include "ModuleInput.h"


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
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);

	ImVec2 size = ImGui::GetWindowSize();
	ImGui::SetCursorPos({ -(App->window->width - size.x) / 2,-(App->window->height - size.y) / 2 });
	App->gui->sceneX = ImGui::GetCursorPosX() + ImGui::GetWindowPos().x;
	App->gui->sceneY = ImGui::GetCursorPosY() + ImGui::GetWindowPos().y;
	App->gui->sceneW = App->window->width;
	App->gui->sceneH = App->window->height;

	ImGui::Image((ImTextureID)App->renderer3D->renderedTexture, { (float)App->window->width, (float)App->window->height }, { 0,1 }, { 1,0 });
	
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
