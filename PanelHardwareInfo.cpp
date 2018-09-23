#include "PanelHardwareInfo.h"
#include "ImGui\imgui.h"

PanelHardwareInfo::PanelHardwareInfo(const char* name): Panel(name)
{
	posX = 725;
	posY = 18;
	width = 354;
	height = 438;
}

PanelHardwareInfo::~PanelHardwareInfo()
{
}

void PanelHardwareInfo::Draw()
{
	ImGui::Begin(name.c_str(), &active);



	ImGui::End();
}
