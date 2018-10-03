#include "PanelConsole.h"
PanelConsole::PanelConsole(const char* name, float posX, float posY, float width, float height, panelAlingnment aligned): Panel(name, posX, posY, width, height, aligned)
{
	active = true;
}

PanelConsole::~PanelConsole()
{
}

void PanelConsole::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove);
	ImGui::TextUnformatted(Buffer.begin());
	
	if (ScrollToBottom)
		ImGui::SetScrollHere(1.0f);
	
	ScrollToBottom = false;

	checkMoved();
	ImGui::End();
}

void PanelConsole::AddLog(const char* logs)
{
	Buffer.appendf(logs);
	ScrollToBottom = true;
}
