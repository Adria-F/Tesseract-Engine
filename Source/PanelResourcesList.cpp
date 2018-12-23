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
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysVerticalScrollbar);

	ImGui::BeginMenuBar();
	switch (currentType)
	{
	case R_MESH:
		ImGui::Text("Meshes");
		break;
	case R_TEXTURE:
		ImGui::Text("Materials");
		break;
	case R_SCENE:
		ImGui::Text("Scenes");
		break;
	case R_ANIMATION:
		ImGui::Text("Animations");
		break;
	}
	ImGui::EndMenuBar();
	if (currentType == R_TEXTURE)
	{
		float width = ImGui::GetWindowContentRegionWidth();
		bool pushed = false;
		if (!showExtra)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 1.0f,0.65f,0.1f,1.0f });
			pushed = true;
		}
		if (ImGui::Button("Textures", { width / 2,20 }))
			showExtra = false;
		if (pushed)
		{
			ImGui::PopStyleColor();
			pushed = false;
		}
		ImGui::SameLine();
		if (showExtra)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, { 1.0f,0.65f,0.1f,1.0f });
			pushed = true;
		}
		if (ImGui::Button("Colors", { width / 2, 20 }))
			showExtra = true;
		if (pushed)
			ImGui::PopStyleColor();
	}

	std::vector<Resource*> resourceList = resources;
	if (currentType == R_TEXTURE && showExtra)
		resourceList = extraResources;

	if (ImGui::Selectable("None"))
	{
		if (callbackComponent != nullptr)
		{
			callbackComponent->onResourceListEvent(0, tag.c_str());
		}
		active = false;
	}

	for (int i = 0; i < resourceList.size(); i++)
	{
		if (ImGui::Selectable(resourceList[i]->GetName()))
		{
			if (callbackComponent != nullptr)
			{
				callbackComponent->onResourceListEvent(resourceList[i]->GetUID(), tag.c_str());
			}
			active = false;
		}
	}
	if (ImGui::IsMouseClicked(0) && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		active = false;

	ImGui::End();
}

void PanelResourcesList::startList(ResType type, int x, int y, Component* callback, const char* tag)
{
	currentType = type;
	resources = App->resources->getResourcesByType(type);
	if (type == R_TEXTURE)
		extraResources = App->resources->getResourcesByType(R_COLOR);
	callbackComponent = callback;
	this->tag = tag;

	ImGui::SetWindowPos(name.c_str(), {(float)x, (float)y });

	active = true;
}
