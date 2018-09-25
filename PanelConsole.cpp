#include "PanelConsole.h"
#include "ImGui\imgui.h"

PanelConsole::PanelConsole(const char* name, float posX, float posY, float width, float height): Panel(name, posX, posY, width, height)
{
	active = true;
}

PanelConsole::~PanelConsole()
{
}

void PanelConsole::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing);
	ImGui::TextUnformatted(Buffer.begin());
	
	if (ScrollToBottom)
		ImGui::SetScrollHere(1.0f);
	
	ScrollToBottom = false;

	ImGui::End();
}

void PanelConsole::AddLog(const char* logs)
{
	Buffer.appendf(logs);
	ScrollToBottom = true;
}
