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
	
	if(!App->scene_intro->GameMode)
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.95f,0.5f,0.0f,0.7f });
	else
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.5f,0.5f,0.95f,0.7f });

	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.0f,1.0f,1.0f,0.2f });
	

	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 85)/2);
	if (ImGui::ArrowButton("play", ImGuiDir_Right))
	{
		if (!App->scene_intro->GameMode)
		{
			App->Save();
			App->scene_intro->GameMode = true;
		}
		else
		{
			App->Load();
			App->scene_intro->GameMode = false;
		}
	}

	ImGui::SameLine();
	if ( ImGui::Button("||", {23,19}) && App->scene_intro->GameMode )
	{
		//dt=0;
	}
	ImGui::PopStyleColor(2);
	ImGui::SameLine();

	if (!App->scene_intro->GameMode)
		ImGui::PushStyleColor(ImGuiCol_Button, { 1.0f,1.0f,1.0f,0.2f });
	else
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.5f,0.5f,0.95f,0.7f });
	
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.0f,1.0f,1.0f,0.2f });

	if ( ImGui::Button("->", { 23,19 }) && App->scene_intro->GameMode )
	{
		
	}
	ImGui::PopStyleColor(2);
	ImGui::EndMenuBar();

	

	ImGui::End();
}
