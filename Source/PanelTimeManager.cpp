#include "Application.h"
#include "PanelTimeManager.h"
#include "ModuleScene.h"
#include "ModuleSceneLoader.h"




PanelTimeManager::PanelTimeManager(const char* name) : Panel(name)
{
	active = true;
}


PanelTimeManager::~PanelTimeManager()
{
	
}

void PanelTimeManager::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImVec2(100,50));

	ImGui::SameLine();
	if (ImGui::ArrowButton("play", ImGuiDir_Right))
	{
		//App->scene_loader->saveScene();
	}
	ImGui::SameLine();
	if (ImGui::Button("||"))
	{
		//dt=0;
	}
	ImGui::SameLine();
	if(ImGui::Button("| |"))
	{
		//App->scene_loader->loadScene();
	}

	ImGui::End();
}
