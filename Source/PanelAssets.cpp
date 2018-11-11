#include "PanelAssets.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"

PanelAssets::PanelAssets(const char* name) : Panel(name)
{
	active = true;

	current_path = ASSETS_FOLDER;
	App->fileSystem->getFilesAt(ASSETS_FOLDER, elements, nullptr, "meta");
}

PanelAssets::~PanelAssets()
{
	clearElements();
}

void PanelAssets::Draw()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_MenuBar);

	float folderWidth = (float)App->gui->folder->width;

	//Buttons
	ImGui::BeginMenuBar();
	ImGui::PushStyleColor(ImGuiCol_Button, { 0.9f,0.45f,0.0f,0.7f });
	if (ImGui::ArrowButton("back", ImGuiDir_Left))
	{
		clearElements();
		App->fileSystem->splitPath(current_path.c_str(), &current_path, nullptr, nullptr);
		App->fileSystem->getFilesAt(current_path.c_str(), elements, nullptr, "meta");
	}
	ImGui::Text(current_path.c_str());
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 40);
	if (ImGui::Button("Reload"))
	{
		clearElements();
		current_path = ASSETS_FOLDER;
		App->fileSystem->getFilesAt(ASSETS_FOLDER, elements, nullptr, "meta");
	}
	ImGui::PopStyleColor();
	ImGui::EndMenuBar();

	//Files
	ImGui::PushStyleColor(ImGuiCol_Button, { 1.0f,1.0f,1.0f,0.2f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f,0.37f,0.0f,0.7f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.95f,0.5f,0.0f,0.7f });
	ImGui::PushStyleColor(ImGuiCol_Header, { 0.0f,0.0f,0.0f,0.0f });
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, { 0.0f,0.0f,0.0f,0.0f });
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.0f,0.0f,0.0f,0.0f });

	int i = 0;
	int rowCount = 1;
	float leftMargin = ImGui::GetCursorPosX();
	float currLine = ImGui::GetCursorPosY();
	for (std::list<assetsElement*>::iterator it_e = elements.begin(); it_e != elements.end(); it_e++)
	{
		//Set proper cursor position
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
				if (current_path.size() > 0 && current_path.back() != '/')
					current_path += '/';
				current_path += (*it_e)->name;
				clearElements();
				App->fileSystem->getFilesAt(current_path.c_str(), elements, nullptr, "meta");
				ImGui::PopID();
				break;
			}
			else
			{
				//Load file
			}
		}
		//Draw name
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