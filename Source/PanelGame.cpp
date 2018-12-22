#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "PanelGame.h"
#include "ModuleGUI.h"
#include "ModuleScene.h"
#include "ModuleSceneLoader.h"
#include "PanelScene.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
#include "ComponentCamera.h"
#include "GameObject.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

PanelGame::PanelGame(const char* name) : Panel(name)
{
	active = true;
}


PanelGame::~PanelGame()
{
}

void PanelGame::Draw()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar;
	ImGui::Begin(name.c_str(), &active, flags);

	ImVec2 newSize = ImGui::GetWindowContentRegionMax();
	newSize.y += 30;
	if (newSize != size)
	{	
		size = newSize;
		resizedLastFrame = true;
	}

	if (App->scene_intro->activeCamera != nullptr)
	{
		float AR = App->scene_intro->activeCamera->camera->frustum.AspectRatio();
		if (AR != lastAR)
		{
			lastAR = AR;
			resizedLastFrame = true;
		}

		if (resizedLastFrame)
		{
			resizedLastFrame = false;

			viewSize.x = size.x;
			viewSize.y = viewSize.x / AR;

			if (viewSize.y > size.y)
			{
				viewSize.y = size.y;
				viewSize.x = viewSize.y*AR;
			}
		}

		ImGui::SetCursorPos({ (size.x - viewSize.x) / 2, (size.y - viewSize.y) / 2 });
		ImGui::Image((ImTextureID)App->renderer3D->GamerenderedTexture, { (float)viewSize.x, (float)viewSize.y }, { 0,1 }, { 1,0 });
	}
	else
	{
		ImGui::SetCursorPos({ size.x / 2 - 65,size.y / 2 - 15 });
		ImGui::Text("No Main Camera Defined");
	}

	ImGui::End();
}
