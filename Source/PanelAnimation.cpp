#include "Application.h"
#include "ModuleInput.h"
#include "PanelAnimation.h"

PanelAnimation::PanelAnimation(const char* name):Panel(name)
{
	active = true;

	zoom= 50;
	numFrames = 200;
	recSize = 700;
	speed = 20.0f;
}


PanelAnimation::~PanelAnimation()
{
}

void PanelAnimation::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_HorizontalScrollbar);

	//Animation bar Progress
	ImGui::SetCursorPosX(85);
	ImGui::ProgressBar(0.1f, { 700,0 });

	//Animation typeos of Keys
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

	//Animation TimeLine
	ImGui::BeginChild("TimeLine", ImVec2(700, 180), true, ImGuiWindowFlags_AlwaysAutoResize);
	ImVec2 p = ImGui::GetCursorScreenPos();

	for (int i = 0; i < numFrames; i++)
	{
		ImGui::BeginGroup();
				
		ImGui::GetWindowDrawList()->AddLine({ p.x - barMovement.x,p.y }, ImVec2(p.x - barMovement.x, p.y + 135), IM_COL32(100, 100, 100, 255), 1.0f);

		char frame[8];
		sprintf(frame, "%01d", i);
		ImVec2 aux = { p.x - barMovement.x + 3,p.y };
		ImGui::GetWindowDrawList()->AddText(aux, ImColor(1.0f, 1.0f, 1.0f, 1.0f), frame);
		ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x - barMovement.x + 1, p.y + 50), 6.0f, ImColor(0.0f, 0.0f, 1.0f, 1.0f));

		p = { p.x + zoom,p.y };

		ImGui::EndGroup();

		ImGui::SameLine();

	}

	ImGui::NewLine();

	ImVec2 leftLimit = { ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 130 };
	ImVec2 rightLimit = { ImGui::GetCursorScreenPos().x+(numFrames>10?recSize/10:recSize/numFrames), ImGui::GetCursorScreenPos().y + 145 };
	
	bool mouse = ImGui::IsMouseHoveringRect({ leftLimit.x + mouseMovement.x , leftLimit.y}, {rightLimit.x + mouseMovement.x , rightLimit.y });

	ImGui::GetWindowDrawList()->AddRectFilled({ leftLimit.x + mouseMovement.x , leftLimit.y }, { rightLimit.x + mouseMovement.x , rightLimit.y }, ImColor(1.0f, 0.7f, 0.3f, 1.0f));


	if (mouse && ImGui::IsMouseDown(0) && dragging == false)
	{
		dragging = true;
	}


	if (dragging && ImGui::IsMouseDown(0))
	{
		if ( leftLimit.x + mouseMovement.x + ImGui::GetMouseDragDelta(0).x > leftLimit.x && rightLimit.x + mouseMovement.x + ImGui::GetMouseDragDelta(0).x < leftLimit.x+700)
		{

			int num = (numFrames - (700.0f / zoom));

			if (ImGui::GetMouseDragDelta(0).x > 0)
			{
				mouseMovement.x +=  (700.0f- (numFrames>10 ? recSize / 10 : recSize / numFrames))/ numFrames;
				barMovement.x += ((numFrames - (700.0f / zoom)) * zoom) / numFrames;
			}
			if (ImGui::GetMouseDragDelta(0).x < 0)
			{
				mouseMovement.x -=(700.0f- (numFrames>10 ? recSize / 10 : recSize / numFrames)) / numFrames;
				barMovement.x -= ((numFrames - (700.0f / zoom)) * zoom) / numFrames;
			}
			
			mouseMovement.y += 0;
			ImGui::ResetMouseDragDelta();
		}
	}
	else
	{
		dragging = false;
	}
	   	 
	ImGui::EndChild();

	ImGui::End();
}