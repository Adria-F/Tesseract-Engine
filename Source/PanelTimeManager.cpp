#include "PanelTimeManager.h"



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
	ImGui::Button("Play");
	ImGui::SameLine();
	ImGui::Button("Pause");
	ImGui::SameLine();
	ImGui::Button("Stop");

	ImGui::End();
}
