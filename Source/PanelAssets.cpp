#include "PanelAssets.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"

PanelAssets::PanelAssets(const char* name) : Panel(name)
{
	active = true;

	App->fileSystem->getFilesAt(ASSETS_FOLDER, elements);
}

PanelAssets::~PanelAssets()
{
	clearElements();
}

void PanelAssets::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_MenuBar);

	float folderWidth = (float)App->gui->folder->width;

	ImGui::BeginMenuBar();
	ImGui::PushStyleColor(ImGuiCol_Button, { 0.9f,0.45f,0.0f,0.7f });
	if (ImGui::Button("Back"))
	{
		if (elementsHistory.size() > 0)
		{
			currElement = elementsHistory.top();
			elementsHistory.pop();
		}
		else
			currElement = nullptr;
	}
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
	if (ImGui::Button("Reload"))
	{
		clearElements();
		App->fileSystem->getFilesAt(ASSETS_FOLDER, elements);
	}
	ImGui::PopStyleColor();
	ImGui::EndMenuBar();

	ImGui::PushStyleColor(ImGuiCol_Button, { 1.0f,1.0f,1.0f,0.2f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f,0.37f,0.0f,0.7f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.95f,0.5f,0.0f,0.7f });
	ImGui::PushStyleColor(ImGuiCol_Header, { 0.0f,0.0f,0.0f,0.0f });
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, { 0.0f,0.0f,0.0f,0.0f });
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.0f,0.0f,0.0f,0.0f });

	std::list<assetsElement*> usingList = elements;
	if (currElement != nullptr)
		usingList = currElement->elements;

	int i = 0;
	int rowCount = 1;
	float leftMargin = ImGui::GetCursorPosX();
	float currLine = ImGui::GetCursorPosY();
	for (std::list<assetsElement*>::iterator it_e = usingList.begin(); it_e != usingList.end(); it_e++)
	{
		if (rowCount == 1)
			currLine = ImGui::GetCursorPosY();
		else
		{
			if (ImGui::GetWindowContentRegionMax().x >= 70 * rowCount)
			{
				ImGui::SetCursorPos({ (float)leftMargin + (50 + 30)*(rowCount - 1), currLine });
			}
			else
			{
				rowCount == 1;
				currLine = ImGui::GetCursorPosY();
			}
		}
		
		ImGui::PushID(i);
		if (ImGui::ImageButton((ImTextureID)(((*it_e)->type == assetsElement::FOLDER) ? App->gui->folder->GL_id : App->gui->file->GL_id), { 50, 50 }))
		{
			if ((*it_e)->type == assetsElement::FOLDER)
			{
				if (currElement != nullptr)
					elementsHistory.push(currElement);
				currElement = (*it_e);
			}
		}
		ImGui::PopID();
		if (rowCount > 1 && ImGui::GetWindowContentRegionMax().x >= 70 * rowCount)
		{
			ImGui::SetCursorPosX((float)leftMargin + (50 + 30)*(rowCount - 1));
		}
		ImGui::Selectable((*it_e)->name.c_str(), false, 0, { 60, 13 });
		rowCount++;
		i++;
	}

	ImGui::PopStyleColor(6);

	ImGui::End();
}

void PanelAssets::pushElement(assetsElement* element)
{
	elements.push_back(element);
}

void PanelAssets::clearElements()
{
	for (std::list<assetsElement*>::iterator it_e = elements.begin(); it_e != elements.end(); it_e++)
	{
		RELEASE(*it_e);
	}
	elements.clear();

	currElement = nullptr;
}