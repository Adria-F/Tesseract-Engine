#include "Application.h"
#include "ModuleInput.h"
#include "PanelAnimation.h"
#include "ResourceAnimation.h"

PanelAnimation::PanelAnimation(const char* name):Panel(name)
{
	active = true;

	zoom= 50;
	numFrames = 100;
	recSize = 700;
	speed = 0.5f;
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

	ImGui::SetCursorPosY(85);

	ImGui::Text("Position");

	ImGui::SetCursorPosY(125);

	ImGui::Text("Scale");

	ImGui::SetCursorPosY(165);
	
	ImGui::Text("Rotation");
	
	ImGui::EndGroup();

	ImGui::SameLine();

	//Animation TimeLine
	ImGui::BeginChild("TimeLine", ImVec2(1000, 180), true, ImGuiWindowFlags_HorizontalScrollbar);
	ImVec2 p = ImGui::GetCursorScreenPos();

	ImGui::InvisibleButton("scrollbar", { numFrames*zoom ,140});
	ImGui::SetCursorScreenPos(p);

	for (int i = 0; i < numFrames; i++)
	{
		ImGui::BeginGroup();
				
		ImGui::GetWindowDrawList()->AddLine({ p.x,p.y }, ImVec2(p.x, p.y + 135), IM_COL32(100, 100, 100, 255), 1.0f);

		char frame[8];
		sprintf(frame, "%01d", i);
		ImVec2 aux = { p.x + 3,p.y };
		ImGui::GetWindowDrawList()->AddText(aux, ImColor(1.0f, 1.0f, 1.0f, 1.0f), frame);

		if (animation != nullptr && selectedBone!=nullptr)
		{
			if (selectedBone->PosKeysTimes[i] == i)
			{
				ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y+35), 6.0f, ImColor(0.0f, 0.0f, 1.0f, 0.5f));
			}
			if (selectedBone->ScaleKeysTimes[i] == i)
			{
				ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 75), 6.0f, ImColor(0.0f, 1.0f, 0.0f, 0.5f));
			}
			if (selectedBone->RotKeysTimes[i] == i)
			{
				ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 115), 6.0f, ImColor(1.0f, 0.0f, 0.0f, 0.5f));
			}
		}

		p = { p.x + zoom,p.y };

		ImGui::EndGroup();

		ImGui::SameLine();

	}

	/*ImGui::NewLine();

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
				mouseMovement.x +=  ((700.0f- (numFrames>10 ? recSize / 10 : recSize / numFrames))/ numFrames)*speed;
				barMovement.x += (((numFrames - (700.0f / zoom)) * zoom) / numFrames)*speed;
			}
			if (ImGui::GetMouseDragDelta(0).x < 0)
			{
				mouseMovement.x -=((700.0f- (numFrames>10 ? recSize / 10 : recSize / numFrames)) / numFrames)*speed;
				barMovement.x -= (((numFrames - (700.0f / zoom)) * zoom) / numFrames)*speed;
			}
			
			mouseMovement.y += 0;
			ImGui::ResetMouseDragDelta();
		}
	}
	else
	{
		dragging = false;
	}

	ImGuiIO io = ImGui::GetIO();

	if (mouse && io.MouseWheel != 0.0f)
	{
		zoom += 5*io.MouseWheel;
	}*/

	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginGroup();
	
	ImGui::BeginChild("All Animations", ImVec2(250, 140), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

	if (animation != nullptr)
	{
		for (int i = 0; i < animation->numBones; i++)
		{
			if (ImGui::Button(animation->bones[i].NodeName.c_str()))
			{
				selectedBone = &animation->bones[i];
			}
		}
	}

	ImGui::EndChild();

	//ImGui::SameLine();

	ImGui::BeginChild("Selected Bone", ImVec2(250, 30), true);

	if (selectedBone != nullptr)
	{
		ImGui::Text(selectedBone->NodeName.c_str());
	}

	ImGui::EndChild();
	ImGui::EndGroup();

	ImGui::End();
}