#include "PanelConsole.h"
PanelConsole::PanelConsole(const char* name): Panel(name)
{
	active = true;
}

PanelConsole::~PanelConsole()
{
}

void PanelConsole::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing);

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
