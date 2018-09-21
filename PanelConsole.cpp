#include "PanelConsole.h"
#include "ImGui\imgui.h"

PanelConsole::PanelConsole(const char* name): Panel(name)
{
	posX = 0;
	posY = 551;
	width = 602;
	height = 170;
}

PanelConsole::~PanelConsole()
{
}

void PanelConsole::Draw()
{
	ImGui::Begin(name.c_str());



	ImGui::End();
}
