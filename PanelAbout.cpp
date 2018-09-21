#include "PanelAbout.h"
#include "ImGui\imgui.h"

PanelAbout::PanelAbout(const char* name) : Panel(name)
{
	posX = 525;
	posY = 250;
	width = 200;
	height = 207;
}

PanelAbout::~PanelAbout()
{
}

void PanelAbout::Draw()
{
	ImGui::Begin(name.c_str(), &active);



	ImGui::End();
}
