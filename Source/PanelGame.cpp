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
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		flags |= ImGuiWindowFlags_NoMove;
	ImGui::Begin(name.c_str(), &active, flags);

	ImVec2 newSize = ImGui::GetWindowSize();
	if (newSize != size)
	{
		resizedLastFrame = true;
		size = newSize;
		float newAR = size.x / size.y;
		App->camera->camera->setAspectRatio(newAR);
		App->renderer3D->changedFOV = true;
	}
	else
		resizedLastFrame = false;

	ImGui::Image((ImTextureID)App->renderer3D->GamerenderedTexture, { (float)size.x, (float)size.y }, { 0,1 }, { 1,0 });

	/*if (App->input->GetMouseButton(SDL_BUTTON_LEFT) != KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_MIDDLE) != KEY_REPEAT)
		App->gui->hoveringScene = ImGui::IsMouseHoveringWindow();*/

	ImGui::End();
}
