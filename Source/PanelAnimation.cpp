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
	progress = 0.0f;
	winSize = 1000.0f;
}


PanelAnimation::~PanelAnimation()
{
}

void PanelAnimation::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_HorizontalScrollbar);

	//Animation bar Progress
	ImGui::SetCursorPosX(85);
	ImGui::ProgressBar((progress/(zoom*numFrames)), { winSize,0 });

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
	ImGui::BeginChild("TimeLine", ImVec2(winSize, 180), true, ImGuiWindowFlags_HorizontalScrollbar);
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImVec2 redbar = ImGui::GetCursorScreenPos();
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

	//RedLine 
	if(!App->GameMode)
		ImGui::GetWindowDrawList()->AddLine({ redbar.x,redbar.y-10 }, ImVec2(redbar.x, redbar.y + 165), IM_COL32(255, 0, 0, 100), 1.0f);
	else
	{
		ImGui::GetWindowDrawList()->AddLine({ redbar.x + progress,redbar.y - 10 }, ImVec2(redbar.x + progress, redbar.y + 165), IM_COL32(255, 0, 0, 255), 1.0f);

		if (!App->GamePaused)
		{
			progress += zoom;
			scrolled = false;
		}

		float framesInWindow = winSize/zoom;

		if (progress!=0 && fmod(progress,winSize) ==0 && !scrolled)
		{
			float scroolPos=ImGui::GetScrollX();
			ImGui::SetScrollX(scroolPos+ winSize);
			scrolled = true;
		}

		if (progress == zoom * numFrames)
		{
			progress = 0.0f;
			ImGui::SetScrollX(0);
		}
	}

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