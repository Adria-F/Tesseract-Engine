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

	if (App->scene_intro->activeCamera != nullptr)
	{
		ImVec2 newSize = ImGui::GetWindowSize();
		if (newSize != size)
		{
			resizedLastFrame = true;
			size = newSize;
			float newAR = size.x / size.y;
			if (App->scene_intro->activeCamera != nullptr)
				App->scene_intro->activeCamera->camera->setAspectRatio(newAR);
			App->renderer3D->changedGameFOV = true;
		}
		else
			resizedLastFrame = false;

		ImGui::Image((ImTextureID)App->renderer3D->GamerenderedTexture, { (float)size.x, (float)size.y }, { 0,1 }, { 1,0 });
	}
	else
	{
		ImVec2 newSize = ImGui::GetWindowSize();
		ImGui::SetCursorPos({ newSize.x / 2 - 65,newSize.y / 2 - 15 });
		ImGui::Text("No Main Camera Defined");
	}

	ImGui::End();
}
