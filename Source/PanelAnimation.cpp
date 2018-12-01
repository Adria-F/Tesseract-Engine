#include "PanelAnimation.h"



PanelAnimation::PanelAnimation(const char* name):Panel(name)
{
	active = true;
}


PanelAnimation::~PanelAnimation()
{
}

void PanelAnimation::Draw()
{
	ImGui::Begin(name.c_str(), &active);

	ImGui::BeginGroup();
	ImGui::Text("Position X");
	ImGui::Text("Position Y");
	ImGui::Text("Position Z");
	ImGui::Text("Rotation X");
	ImGui::Text("Rotation Y");
	ImGui::Text("Rotation Z");
	ImGui::Text("Scale X");
	ImGui::Text("Scale Y");
	ImGui::Text("Scale Z");
	ImGui::EndGroup();

	

	ImGui::SameLine();

	ImGui::BeginChild("TimeLine", ImVec2(700, 170), true,ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_HorizontalScrollbar);
	ImVec2 p = ImGui::GetCursorScreenPos();
	for (int i = 0; i < 20; i++)
	{
		p = { p.x + 50,p.y };
		ImGui::GetWindowDrawList()->AddLine(p, ImVec2(p.x, p.y + 135), IM_COL32(100, 100, 100, 255), 1.0f);
	}

	ImGui::EndChild();


	ImGui::End();
}