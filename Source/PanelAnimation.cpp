#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "PanelAnimation.h"
#include "ComponentAnimation.h"
#include "ModuleResource.h"
#include "ResourceAnimation.h"

PanelAnimation::PanelAnimation(const char* name):Panel(name)
{
	active = true;

	zoom= 25;
	numFrames = 100;
	recSize = 1000;
	speed = 0.5f;
	progress = 0.0f;
	winSize = 1000.0f;
}

bool PanelAnimation::fillInfo()
{
	bool ret = false;
	
	if (App->scene_intro->selected_GO != nullptr)
	{
		compAnimation = (ComponentAnimation*)App->scene_intro->selected_GO->GetComponent(ANIMATION);

		if (compAnimation != nullptr)
		{
			animation = (ResourceAnimation*)App->resources->GetResource(compAnimation->RUID);

			if (animation != nullptr)
			{
				ret = true;
				numFrames = animation->ticks;
			}
		}
	}
	
	return ret;
}


PanelAnimation::~PanelAnimation()
{
}

void PanelAnimation::Draw()
{

	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_HorizontalScrollbar);

	if (fillInfo())
	{
		//Animation bar Progress
		ImGui::SetCursorPosX(85);
		ImGui::ProgressBar((compAnimation->animTime / animation->getDuration()), { winSize,0 });
		ImGui::SameLine();
		if (ImGui::Button("Play"))
		{
			compAnimation->TestPlay = true;
			compAnimation->TestPause = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Pause"))
		{
			compAnimation->TestPause = !compAnimation->TestPause;
			mouseMovement.x = progress;
			buttonPos = progress;
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop") && compAnimation->TestPlay)
		{
			compAnimation->TestPlay = false;
			compAnimation->animTime=0.0f;
			compAnimation->TestPause = false;
		}
		

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
		ImGui::InvisibleButton("scrollbar", { numFrames*zoom ,140 });
		ImGui::SetCursorScreenPos(p);

		for (int i = 0; i < numFrames; i++)
		{
			ImGui::BeginGroup();

			ImGui::GetWindowDrawList()->AddLine({ p.x,p.y }, ImVec2(p.x, p.y + 135), IM_COL32(100, 100, 100, 255), 1.0f);

			char frame[8];
			sprintf(frame, "%01d", i);
			ImVec2 aux = { p.x + 3,p.y };
			ImGui::GetWindowDrawList()->AddText(aux, ImColor(1.0f, 1.0f, 1.0f, 1.0f), frame);

			if (animation != nullptr && selectedBone != nullptr)
			{
				if (selectedBone->PosKeysTimes[i] == i)
				{
					ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(p.x + 1, p.y + 35), 6.0f, ImColor(0.0f, 0.0f, 1.0f, 0.5f));
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
		if (!App->inGameMode() && !compAnimation->TestPlay && !compAnimation->TestPause)
		{
			ImGui::GetWindowDrawList()->AddLine({ redbar.x,redbar.y - 10 }, ImVec2(redbar.x, redbar.y + 165), IM_COL32(255, 0, 0, 100), 1.0f);
			progress = 0.0f;
			ImGui::SetScrollX(0);
		}
		else if(!compAnimation->TestPause)
		{
			float auxprgbar = progress;

			ImGui::GetWindowDrawList()->AddLine({ redbar.x + progress,redbar.y - 10 }, ImVec2(redbar.x + progress, redbar.y + 165), IM_COL32(255, 0, 0, 255), 1.0f);
			
			if (!App->isGamePaused())
			{
				progress = (compAnimation->animTime*animation->ticksXsecond)*zoom;
				scrolled = false;
				LOG("%f", progress);
			}

			float framesInWindow = winSize / zoom;

			if (progress != 0 && progress> winSize && !scrolled)
			{
				float scroolPos = ImGui::GetScrollX();
				ImGui::SetScrollX(scroolPos + winSize);
				scrolled = true;
			}

			if (auxprgbar>progress)
			{
				progress = 0.0f;
				ImGui::SetScrollX(0);
			}
		}

		if (compAnimation->TestPause)
		{
			ImGui::SetCursorPos({ buttonPos,ImGui::GetCursorPosY() + 140 });
			ImGui::PushID("scrollButton");
			ImGui::Button("", { 20, 15 });
			ImGui::PopID();

			if (ImGui::IsItemClicked(0) && dragging == false)
			{
				dragging = true;
				offset = ImGui::GetMousePos().x - ImGui::GetWindowPos().x - buttonPos;
			}

			if (dragging && ImGui::IsMouseDown(0))
			{
				buttonPos = ImGui::GetMousePos().x - ImGui::GetWindowPos().x - offset;
				if (buttonPos < 0)
					buttonPos = 0;
				if (buttonPos > numFrames*zoom - 20)
					buttonPos = numFrames * zoom - 20;

				progress = buttonPos;
				compAnimation->animTime = progress / (animation->ticksXsecond *zoom);
				
			}
			else
			{
				dragging = false;
			}

			ImGui::GetWindowDrawList()->AddLine({ redbar.x + progress,redbar.y - 10 }, ImVec2(redbar.x + progress, redbar.y + 165), IM_COL32(255, 0, 0, 255), 1.0f);
		}

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();

		ImGui::BeginChild("All Animations", ImVec2(250, 140), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

		if (animation != nullptr)
		{
			for (int i = 0; i < animation->numBones; i++)
			{
				if (ImGui::Button(animation->boneTransformations[i].NodeName.c_str()))
				{
					selectedBone = &animation->boneTransformations[i];
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

		ImGui::NewLine();
	}

	ImGui::End();
}