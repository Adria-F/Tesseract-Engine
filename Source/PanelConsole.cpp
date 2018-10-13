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
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_MenuBar);

	ImGui::BeginMenuBar();
	ImGui::PushStyleColor(ImGuiCol_Button, { 0.9f,0.45f,0.0f,0.7f });
	if (ImGui::Button("Clear"))
	{
		Buffer.clear();
	}		
	ImGui::PopStyleColor();
	ImGui::EndMenuBar();

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
