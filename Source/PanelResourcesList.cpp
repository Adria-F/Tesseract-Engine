#include "PanelResourcesList.h"
#include "Application.h"
#include "ModuleResource.h"
#include "Component.h"

#ifdef _DEBUG
//#define TEST_MEMORY_MANAGER
#include "mmgr/mmgr.h"
#endif

PanelResourcesList::PanelResourcesList(const char* name) : Panel(name)
{
}

PanelResourcesList::~PanelResourcesList()
{
}

void PanelResourcesList::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);

	ImGui::BeginMenuBar();
	switch (currentType)
	{
	case R_MESH:
		ImGui::Text("Meshes");
		break;
	case R_MATERIAL:
		ImGui::Text("Textures");
		break;
	case R_SCENE:
		ImGui::Text("Scenes");
		break;
	case R_ANIMATION:
		ImGui::Text("Animations");
		break;
	}
	ImGui::EndMenuBar();

	for (int i = 0; i < resources.size(); i++)
	{
		if (ImGui::Selectable(resources[i]->GetName()))
		{
			if (callbackComponent != nullptr)
			{
				callbackComponent->assignResource(resources[i]->GetUID());
			}
			active = false;
		}
	}
	if (ImGui::IsMouseClicked(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		active = false;

	ImGui::End();
}

void PanelResourcesList::startList(ResType type, int x, int y, Component* callback)
{
	currentType = type;
	resources = App->resources->getResourcesByType(type);
	callbackComponent = callback;

	ImGui::SetWindowPos(name.c_str(), {(float)x, (float)y });

	active = true;
}
