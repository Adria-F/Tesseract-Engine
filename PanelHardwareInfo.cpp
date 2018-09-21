#include "PanelHardwareInfo.h"
#include "ImGui\imgui.h"

PanelHardwareInfo::PanelHardwareInfo(const char* name): Panel(name)
{
	posX = 528;
	posY = 20;
	width = 197;
	height = 230;
}

PanelHardwareInfo::~PanelHardwareInfo()
{
}

void PanelHardwareInfo::Draw()
{
	ImGui::Begin(name.c_str());



	ImGui::End();
}
